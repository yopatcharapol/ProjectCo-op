param(
    [Parameter(Mandatory = $true)][string]$InputManifest,
    [Parameter(Mandatory = $true)][string]$OutputDirectory
)

$ErrorActionPreference = 'Stop'
$manifestInput = [IO.File]::ReadAllText((Resolve-Path -LiteralPath $InputManifest).Path) | ConvertFrom-Json
$captures = @($manifestInput.captures)
if (-not $captures.Count) { throw 'No captures in input manifest.' }
if (@($captures.capture_id | Sort-Object -Unique).Count -ne $captures.Count) { throw 'Capture IDs must be unique.' }
foreach ($capture in $captures) {
    if ($capture.capture_id -notmatch '^[A-Za-z0-9_-]+$') { throw 'Capture ID must be a safe directory name.' }
    if (-not (Test-Path -LiteralPath $capture.source -PathType Leaf)) { throw "Source missing: $($capture.capture_id)" }
}
if (Test-Path -LiteralPath $OutputDirectory) { throw 'Output directory already exists; choose a new path to preserve prior imports.' }
$null = New-Item -ItemType Directory -Path $OutputDirectory
$root = (Resolve-Path -LiteralPath $OutputDirectory).Path
$utf8 = New-Object Text.UTF8Encoding($false)
$columns = 'record_type,schema_version,session_id,trial_id,device_label,condition,scan_mode,timestamp_ms,report_id,mac,addr_type,rssi_dbm,event_type,adv_len,scan_rsp_len,adv_hex,scan_rsp_hex,event,detail,dropped_reports'.Split(',')
$allReports = @(); $captureIndex = @()
$provenance = [ordered]@{
    schema_version=1; pilot_id=$manifestInput.pilot_id; experiment_date=$manifestInput.experiment_date
    date_basis='User session date; serial host clock has no date or timezone embedded'
    status='importing'; captures=@(); excluded=@()
}
$provenancePath = Join-Path $root 'provenance.json'
function Save-Provenance {
    [IO.File]::WriteAllText($provenancePath, ($provenance | ConvertTo-Json -Depth 10), $utf8)
}
Save-Provenance

try {
    foreach ($capture in $captures) {
        $directory = Join-Path $root $capture.capture_id
        $null = New-Item -ItemType Directory -Path $directory
        $sourcePath = (Resolve-Path -LiteralPath $capture.source).Path
        $rawPath = Join-Path $directory 'serial_original.txt'
        [IO.File]::Copy($sourcePath, $rawPath, $false)
        $sourceHash = (Get-FileHash -LiteralPath $sourcePath -Algorithm SHA256).Hash
        if ((Get-FileHash -LiteralPath $rawPath -Algorithm SHA256).Hash -ne $sourceHash) { throw 'Copied raw log differs.' }
        $parsed = @(); $lineNumber = 0
        foreach ($line in [IO.File]::ReadAllLines($rawPath)) {
            $lineNumber++
            if ($line -notmatch '^(?<clock>\d{2}:\d{2}:\d{2}\.\d{3}) -> (?<payload>.*)$') {
                if ($line.Trim()) { throw "Unrecognized source line $lineNumber in $($capture.capture_id)" }
                continue
            }
            $clock = $Matches.clock; $payload = $Matches.payload
            if ($payload.StartsWith('record_type,') -or -not $payload.Trim()) { continue }
            if ([regex]::Matches($payload, '(?:^|,)("(?:[^"]|"")*"|[^,]*)').Count -ne 20) {
                throw "Malformed CSV at $($capture.capture_id):$lineNumber"
            }
            $row = $payload | ConvertFrom-Csv -Header $columns
            if ($row.schema_version -ne '1' -or $row.record_type -notin @('EVENT','REPORT')) { throw 'Unsupported logger record.' }
            $parsed += [pscustomobject]@{line_number=$lineNumber;host_time=$clock;row=$row}
        }
        $starts = @($parsed | Where-Object { $_.row.event -eq 'TRIAL_BEGIN' -and $_.row.condition -eq $capture.condition })
        if ($starts.Count -ne 1) { throw "Expected one selected trial in $($capture.capture_id)" }
        $start = $starts[0]
        $ends = @($parsed | Where-Object {
            $_.line_number -ge $start.line_number -and $_.row.event -eq 'TRIAL_END' -and
            $_.row.session_id -eq $start.row.session_id -and $_.row.trial_id -eq $start.row.trial_id
        })
        if ($ends.Count -ne 1 -or $ends[0].row.detail -notmatch '^DURATION_COMPLETE;received_reports=(\d+)$') {
            throw "No complete timed trial in $($capture.capture_id)"
        }
        $expectedReports = [int]$Matches[1]
        $end = $ends[0]
        $selected = @($parsed | Where-Object { $_.line_number -ge $start.line_number -and $_.line_number -le $end.line_number })
        if (@($selected | Where-Object { $_.row.event -eq 'BOOT' -or $_.row.condition -ne $capture.condition -or $_.row.session_id -ne $start.row.session_id -or $_.row.trial_id -ne $start.row.trial_id }).Count) {
            throw 'Mixed conditions, reset or mixed trials within selected window.'
        }
        $scanStarts = @($selected | Where-Object { $_.row.event -eq 'SCAN_STARTED' })
        if ($scanStarts.Count -ne 1) { throw 'Missing or repeated scan start.' }
        $scanStart = $scanStarts[0]
        $duration = ([long]$end.row.timestamp_ms - [long]$scanStart.row.timestamp_ms) / 1000.0
        if ([math]::Abs($duration - 30) -gt 0.1) { throw 'Pilot expects a complete 30-second trial.' }
        $reports = @($selected | Where-Object { $_.row.record_type -eq 'REPORT' })
        if ($reports.Count -ne $expectedReports) { throw 'REPORT rows do not match final receive count.' }
        for ($i = 0; $i -lt $reports.Count; $i++) {
            $report = $reports[$i].row
            if ([int]$report.report_id -ne $i + 1) { throw 'Missing, duplicated or reordered report ID.' }
            if ($report.adv_hex -notmatch '^(?:[0-9A-Fa-f]{2})*$' -or $report.scan_rsp_hex -notmatch '^(?:[0-9A-Fa-f]{2})*$' -or
                $report.adv_hex.Length -ne 2 * [int]$report.adv_len -or $report.scan_rsp_hex.Length -ne 2 * [int]$report.scan_rsp_len) { throw 'Invalid payload bytes or lengths.' }
            $joined = [ordered]@{
                capture_id=$capture.capture_id;state_label=$capture.state_label
                source_line=$reports[$i].line_number;host_time=$reports[$i].host_time
                elapsed_ms=([long]$report.timestamp_ms - [long]$scanStart.row.timestamp_ms)
            }
            foreach ($column in $columns) { $joined[$column] = $report.$column }
            $allReports += [pscustomobject]$joined
        }
        $details = @($selected | Where-Object { $_.row.event -eq 'AD_DETAILS' })
        $warnings = @($details | Where-Object { $_.row.detail -notmatch ';decode_warnings=0$' }).Count
        $dropped = ($selected | ForEach-Object { [long]$_.row.dropped_reports } | Measure-Object -Maximum).Maximum
        $selected.row | Export-Csv -LiteralPath (Join-Path $directory 'capture.csv') -NoTypeInformation -Encoding UTF8
        $metadata = [ordered]@{
            schema_version=1;capture_id=$capture.capture_id;state_label=$capture.state_label
            session_id=$start.row.session_id;trial_id=$start.row.trial_id;condition=$capture.condition
            device_label=$start.row.device_label;scan_mode=$start.row.scan_mode
            requested_duration_seconds=30;observed_duration_seconds=$duration;completed=$true
            observed_reports=$reports.Count;last_dropped_reports=$dropped
            phone_os='UNKNOWN';core_version='Not embedded in log; local compile target is arduino:esp32 2.0.18-arduino.5'
            notes=$capture.notes;host_start=$scanStart.host_time;host_end=$end.host_time
            source_sha256=$sourceHash;source_attachment_id=(Split-Path (Split-Path $sourcePath -Parent) -Leaf)
            raw_file='serial_original.txt';source_window_lines=@($start.line_number,$end.line_number)
            ad_details_rows=$details.Count;decode_warning_rows=$warnings
        }
        [IO.File]::WriteAllText((Join-Path $directory 'capture.metadata.json'), ($metadata | ConvertTo-Json -Depth 5), $utf8)
        $captureIndex += [pscustomobject]@{
            capture_id=$capture.capture_id;state_label=$capture.state_label;host_start=$scanStart.host_time
            host_end=$end.host_time;duration_s=$duration;report_rows=$reports.Count
            ad_details_rows=$details.Count;decode_warning_rows=$warnings;dropped_reports=$dropped
            source_sha256=$sourceHash;first_source_line=$start.line_number;last_source_line=$end.line_number
        }
        $provenance.captures += $metadata
        Save-Provenance
    }
    foreach ($excluded in @($manifestInput.excluded)) {
        if (-not $excluded) { continue }
        if ($excluded.capture_id -notmatch '^[A-Za-z0-9_-]+$') { throw 'Invalid excluded capture ID.' }
        $excludedDirectory = Join-Path $root 'excluded'
        if (-not (Test-Path -LiteralPath $excludedDirectory)) { $null=New-Item -ItemType Directory -Path $excludedDirectory }
        $excludedRaw = Join-Path $excludedDirectory ($excluded.capture_id + '.txt')
        [IO.File]::Copy((Resolve-Path -LiteralPath $excluded.source).Path, $excludedRaw, $false)
        $excludedHash = (Get-FileHash -LiteralPath $excluded.source -Algorithm SHA256).Hash
        if ((Get-FileHash -LiteralPath $excludedRaw -Algorithm SHA256).Hash -ne $excludedHash) { throw 'Excluded source copy differs.' }
        $provenance.excluded += [pscustomobject]@{capture_id=$excluded.capture_id;reason=$excluded.reason;source_sha256=$excludedHash;raw_file=('excluded/' + $excluded.capture_id + '.txt')}
    }
    $captureIndex | Export-Csv -LiteralPath (Join-Path $root 'capture_index.csv') -NoTypeInformation -Encoding UTF8
    $allReports | Export-Csv -LiteralPath (Join-Path $root 'all_reports.csv') -NoTypeInformation -Encoding UTF8
    & (Join-Path $PSScriptRoot 'summarize_ble_session.ps1') -SessionDirectory $root
    $summary = @(Import-Csv -LiteralPath (Join-Path $root 'summary.csv') -Encoding UTF8)
    if ($summary.Count -ne $captures.Count -or @($summary | Where-Object { [int]$_.invalid_rows -gt 0 -or [int]$_.ad_parse_errors -gt 0 }).Count) { throw 'Summary validation failed.' }
    $provenance.status='validated'
    Save-Provenance
    Write-Host ('Validated captures: ' + $captures.Count + '; REPORT rows: ' + $allReports.Count)
} catch {
    $provenance.status='failed'
    Save-Provenance
    throw
}
