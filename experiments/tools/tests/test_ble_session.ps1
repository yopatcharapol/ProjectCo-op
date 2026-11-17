$ErrorActionPreference = 'Stop'
$toolsRoot = Split-Path $PSScriptRoot -Parent
$testRoot = Join-Path ([IO.Path]::GetTempPath()) ('ble-session-test-' + [guid]::NewGuid().ToString('N'))
$null = New-Item -ItemType Directory -Path $testRoot
$utf8 = New-Object Text.UTF8Encoding($false)
function Assert($Condition, [string]$Message) { if (-not $Condition) { throw $Message } }
function Write-Json([string]$Path, $Value) { [IO.File]::WriteAllText($Path, ($Value | ConvertTo-Json -Depth 8), $utf8) }

$plan = @(& (Join-Path $toolsRoot 'run_ble_session.ps1') -PlanOnly -Repeat 2)
Assert ($plan.Count -eq 10) 'Expected five ordered states per repetition'
Assert (@($plan.trial_id | Sort-Object -Unique).Count -eq 10) 'Trial IDs must be unique across repetitions'
Assert (($plan[0..4].condition -join ',') -eq 'BT_OFF,BT_ON_HOME,SHARING_ON,SCREEN_LOCKED,SHARING_ON') 'Condition sequence is incorrect'

# Exercise the real AD decoder with literal byte fixtures, not substring matching.
$summaryRoot = Join-Path $testRoot 'decoder'
$null = New-Item -ItemType Directory -Path $summaryRoot
$fixtures = @(
    @{adv='02010603094D4505FF0600AABB020AF6';rsp='';rssi='-50'},
    @{adv='0516F3FEAABB';rsp='';rssi='-60'},
    @{adv='07FF060016F3FE00';rsp='';rssi='-70'},
    @{adv='';rsp='0516F3FECCDD';rssi='-80'},
    @{adv='05FF0600';rsp='';rssi='-90'},
    @{adv='GG';rsp='';rssi='-80'}
)
$fixtureRows = @()
$number = 0
foreach ($fixture in $fixtures) {
    $number++
    $fixtureRows += [pscustomobject][ordered]@{
        record_type='REPORT';schema_version=1;session_id='fixture';trial_id='trial01'
        device_label='TEST';condition='SHARING_ON';scan_mode='active';timestamp_ms=(1000 * $number)
        report_id=$number;mac='AA:BB:CC:DD:EE:FF';addr_type=1;rssi_dbm=$fixture.rssi;event_type=3
        adv_len=($fixture.adv.Length / 2);scan_rsp_len=($fixture.rsp.Length / 2)
        adv_hex=$fixture.adv;scan_rsp_hex=$fixture.rsp;event='';detail='';dropped_reports=0
    }
}
$fixtureRows | Export-Csv -LiteralPath (Join-Path $summaryRoot 'one.csv') -NoTypeInformation -Encoding UTF8
Write-Json (Join-Path $summaryRoot 'one.metadata.json') @{
    session_id='fixture';trial_id='trial01';condition='SHARING_ON';completed=$true
    requested_duration_seconds=30;scan_mode='active';last_dropped_reports=0;notes='fixture'
}
($fixtureRows[0] | ConvertTo-Csv -NoTypeInformation)[0] | Set-Content -LiteralPath (Join-Path $summaryRoot 'empty.csv') -Encoding UTF8
Write-Json (Join-Path $summaryRoot 'empty.metadata.json') @{
    session_id='empty';trial_id='trial02';condition='BT_OFF';completed=$false
    requested_duration_seconds=30;scan_mode='passive';last_dropped_reports=0;notes='incomplete fixture'
}
& (Join-Path $toolsRoot 'summarize_ble_session.ps1') -SessionDirectory $summaryRoot
$summary = @(Import-Csv -LiteralPath (Join-Path $summaryRoot 'summary.csv'))
$trial = $summary | Where-Object trial_id -eq 'trial01'
Assert ($trial.report_rows -eq '6' -and $trial.valid_rows -eq '5' -and $trial.invalid_rows -eq '1') 'Invalid payload row was not flagged'
Assert ($trial.fef3_report_rows -eq '2') 'FEF3 must decode from service AD fields, including scan response, not manufacturer bytes'
Assert ($trial.ad_parse_errors -eq '1') 'Truncated AD element was not flagged'
$empty = $summary | Where-Object trial_id -eq 'trial02'
Assert ($empty.completed -eq 'False' -and $empty.report_rows -eq '0') 'Incomplete zero-report trial must remain visible'
$address = Import-Csv -LiteralPath (Join-Path $summaryRoot 'addresses.csv')
Assert ($address.name_hints -eq 'ME') 'Local-name decoding failed'
Assert ($address.manufacturer_ids -eq '0x0006') 'Manufacturer ID little-endian decoding failed'
Assert ($address.service_uuids16 -eq '0xFEF3') 'Service UUID decoding failed'
Assert ($address.advertised_tx_power_dbm -eq '-10') 'Signed advertised TX power decoding failed'
Assert ([double]$address.rssi_mean_dbm -eq -70) 'RSSI mean is incorrect'

# Exercise orchestration at its capture boundary: fake only the physical UART,
# run the real planner, state manifest, summarizer, packaging and failure gates.
$runnerRoot = Join-Path $testRoot 'runner'
$null = New-Item -ItemType Directory -Path $runnerRoot
Copy-Item -LiteralPath (Join-Path $toolsRoot 'run_ble_session.ps1'),(Join-Path $toolsRoot 'summarize_ble_session.ps1') -Destination $runnerRoot
$fakeCapture = @'
param($Port,$Condition,$Mode,$DurationSeconds,$DeviceLabel,$TrialId,$PhoneOs,$CoreVersion,$Notes,$OutputDirectory)
$null = New-Item -ItemType Directory -Path $OutputDirectory -Force
$sessionId = 'fixture_' + $TrialId
$metadata = @{
    session_id=$sessionId;trial_id=$TrialId;condition=$Condition;completed=$true
    requested_duration_seconds=$DurationSeconds;scan_mode=$Mode;last_dropped_reports=0
    notes=$Notes;observed_reports=0
}
if ((Test-Path -LiteralPath (Join-Path $PSScriptRoot 'fail-second')) -and $TrialId -eq 'r01_s02') { $metadata.completed=$false }
$metadata | ConvertTo-Json | Set-Content -LiteralPath (Join-Path $OutputDirectory 'fake.metadata.json') -Encoding UTF8
'record_type,schema_version,session_id,trial_id,device_label,condition,scan_mode,timestamp_ms,report_id,mac,addr_type,rssi_dbm,event_type,adv_len,scan_rsp_len,adv_hex,scan_rsp_hex,event,detail,dropped_reports' | Set-Content -LiteralPath (Join-Path $OutputDirectory 'fake.csv') -Encoding UTF8
'called' | Add-Content -LiteralPath (Join-Path $PSScriptRoot 'calls.txt')
'@
[IO.File]::WriteAllText((Join-Path $runnerRoot 'capture_ble_trial.ps1'), $fakeCapture, $utf8)
$global:BleSessionTestQuit = $false
function Read-Host { param([string]$Prompt) if ($global:BleSessionTestQuit) { return 'q' }; return '' }
$runner = Join-Path $runnerRoot 'run_ble_session.ps1'
$completeRoot = Join-Path $testRoot 'complete'
& $runner -Port 'COM_TEST' -DurationSeconds 10 -SettleSeconds 0 -OutputRoot $completeRoot
$manifestFile = Get-ChildItem -LiteralPath $completeRoot -Recurse -Filter 'session.json' -File
$manifest = Get-Content -LiteralPath $manifestFile.FullName -Raw -Encoding UTF8 | ConvertFrom-Json
Assert ($manifest.status -eq 'completed') 'Successful session not marked completed'
Assert (@($manifest.steps | Where-Object status -eq 'completed').Count -eq 5) 'Expected five completed captures'
Assert (@(Get-ChildItem -LiteralPath $completeRoot -Filter '*.zip').Count -eq 1) 'Completed session ZIP missing'
Assert ((Get-Content -LiteralPath (Join-Path $runnerRoot 'calls.txt')).Count -eq 5) 'Expected one capture per state'

Set-Content -LiteralPath (Join-Path $runnerRoot 'fail-second') -Value 'fixture'
$failureRoot = Join-Path $testRoot 'failure'
$failed = $false
try { & $runner -Port 'COM_TEST' -DurationSeconds 10 -SettleSeconds 0 -OutputRoot $failureRoot } catch { $failed=$true }
Assert $failed 'Incomplete second capture must stop the session'
$manifestFile = Get-ChildItem -LiteralPath $failureRoot -Recurse -Filter 'session.json' -File
$manifest = Get-Content -LiteralPath $manifestFile.FullName -Raw -Encoding UTF8 | ConvertFrom-Json
Assert ($manifest.status -eq 'failed' -and $manifest.steps[0].status -eq 'completed' -and $manifest.steps[1].status -eq 'failed' -and $manifest.steps[2].status -eq 'pending') 'Failed capture advanced the plan or discarded prior progress'
Assert (@(Get-ChildItem -LiteralPath $failureRoot -Filter '*.zip').Count -eq 0) 'Incomplete session must not produce a completed ZIP'

$global:BleSessionTestQuit = $true
$quitRoot = Join-Path $testRoot 'quit'
& $runner -Port 'COM_TEST' -SettleSeconds 0 -OutputRoot $quitRoot
$manifestFile = Get-ChildItem -LiteralPath $quitRoot -Recurse -Filter 'session.json' -File
$manifest = Get-Content -LiteralPath $manifestFile.FullName -Raw -Encoding UTF8 | ConvertFrom-Json
Assert ($manifest.status -eq 'stopped_by_user') 'Quit before first trial must be preserved without opening capture'
Write-Host 'PASS: plan, payload decoding, zero-report/incomplete data, full orchestration/ZIP, failure gate, and quit'
Write-Host ('Test artifacts: ' + $testRoot)
