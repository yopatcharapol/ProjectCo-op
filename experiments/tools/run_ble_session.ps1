param(
    [string]$Port,
    [ValidateRange(10, 600)][int]$DurationSeconds = 30,
    [ValidateRange(1, 10)][int]$Repeat = 1,
    [ValidateRange(0, 60)][int]$SettleSeconds = 5,
    [ValidateSet('passive', 'active')][string]$Mode = 'passive',
    [string]$PhoneOs = 'UNKNOWN',
    [string]$CoreVersion = '2.0.18-arduino.5',
    [ValidatePattern('^[A-Za-z0-9_-]{1,40}$')][string]$DeviceLabel = 'REDMI_PAD_PRO',
    [string]$OutputRoot = (Join-Path $PSScriptRoot '../raw_logs/ble_sessions'),
    [switch]$PlanOnly
)

$ErrorActionPreference = 'Stop'
$phases = @(
    @{condition = 'BT_OFF'; title = 'ปิด Bluetooth'; instruction = 'ปิด Bluetooth ใน Settings แล้ววางเครื่องเป้าหมายที่จุดเดิม'},
    @{condition = 'BT_ON_HOME'; title = 'เปิด Bluetooth หน้าโฮม'; instruction = 'เปิด Bluetooth กลับหน้าโฮม ยังไม่เปิด Quick Share และคงจอเปิดไว้'},
    @{condition = 'SHARING_ON'; title = 'เปิด Quick Share พร้อมรับ'; instruction = 'เปิดหน้า Quick Share แบบรับไฟล์ ให้ทุกคนมองเห็น คงจอเปิดไว้'},
    @{condition = 'SCREEN_LOCKED'; title = 'ล็อกหน้าจอ'; instruction = 'คง Bluetooth และการตั้งค่า Quick Share เดิม กดล็อกจอโดยไม่ขยับเครื่อง'},
    @{condition = 'SHARING_ON'; title = 'ปลดล็อกกลับหน้า Quick Share'; instruction = 'ปลดล็อก กลับหน้า Quick Share แบบรับไฟล์ ให้ทุกคนมองเห็น โดยไม่ขยับเครื่อง'}
)
$plan = @()
for ($round = 1; $round -le $Repeat; $round++) {
    for ($phaseIndex = 0; $phaseIndex -lt $phases.Count; $phaseIndex++) {
        $phase = $phases[$phaseIndex]
        $plan += [pscustomobject][ordered]@{
            trial_id = ('r{0:00}_s{1:00}' -f $round, ($phaseIndex + 1))
            condition = $phase.condition; title = $phase.title; instruction = $phase.instruction
            duration_seconds = $DurationSeconds; status = 'pending'; confirmed_utc = $null
            directory = $null; error = $null
        }
    }
}
if ($PlanOnly) { $plan; return }

$captureScript = Join-Path $PSScriptRoot 'capture_ble_trial.ps1'
$summaryScript = Join-Path $PSScriptRoot 'summarize_ble_session.ps1'
foreach ($requiredPath in @($captureScript, $summaryScript)) {
    if (-not (Test-Path -LiteralPath $requiredPath -PathType Leaf)) { throw "Missing helper: $requiredPath" }
}
if (-not $Port) {
    $ports = @([System.IO.Ports.SerialPort]::GetPortNames())
    if ($ports.Count -eq 1) { $Port = $ports[0] }
    else {
        Write-Host ('พอร์ตที่พบ: ' + ($ports -join ', '))
        $Port = Read-Host 'พิมพ์พอร์ตของ Nano ESP32 เช่น COM5'
    }
}
if ([string]::IsNullOrWhiteSpace($Port)) { throw 'ต้องระบุพอร์ตของบอร์ด' }

$sessionName = 'BLE_' + (Get-Date -Format 'yyyyMMdd_HHmmss') + '_' + [guid]::NewGuid().ToString('N').Substring(0, 8)
$null = New-Item -ItemType Directory -Path $OutputRoot -Force
$sessionDirectory = Join-Path (Resolve-Path -LiteralPath $OutputRoot).Path $sessionName
$null = New-Item -ItemType Directory -Path $sessionDirectory
$utf8 = New-Object System.Text.UTF8Encoding($false)
$manifest = [ordered]@{
    schema_version = 1; session_name = $sessionName; port = $Port
    device_label = $DeviceLabel; phone_os = $PhoneOs; core_version = $CoreVersion
    mode = $Mode; settle_seconds = $SettleSeconds; status = 'running'
    started_utc = [DateTime]::UtcNow.ToString('o'); ended_utc = $null; steps = $plan
}
function Save-Manifest {
    [System.IO.File]::WriteAllText((Join-Path $sessionDirectory 'session.json'), ($manifest | ConvertTo-Json -Depth 8), $utf8)
}
Save-Manifest
Write-Host 'ปิด Arduino Serial Monitor ก่อน โปรแกรมนี้จะเปิดพอร์ตและบันทึกให้เอง'
Write-Host ('ชุดนี้มี ' + $plan.Count + ' รอบ รอบละ ' + $DurationSeconds + ' วินาที')
Write-Host 'คงตำแหน่งเครื่องเป้าหมาย Wi-Fi และอุปกรณ์รอบตัวไว้เหมือนเดิม เปลี่ยนสถานะเครื่องตามที่หน้าจอแจ้ง'
Write-Host ('โฟลเดอร์ผล: ' + $sessionDirectory)

try {
    foreach ($step in $plan) {
        Write-Host ''
        Write-Host ($step.trial_id + ': ' + $step.title) -ForegroundColor Cyan
        Write-Host $step.instruction
        $answer = Read-Host 'ทำบนเครื่องเป้าหมายเสร็จแล้วกด Enter เพื่อเริ่ม หรือพิมพ์ q เพื่อจบชุด'
        if ($answer.Trim().ToLowerInvariant() -eq 'q') {
            $manifest.status = 'stopped_by_user'
            break
        }
        $step.confirmed_utc = [DateTime]::UtcNow.ToString('o')
        $step.status = 'running'
        $step.directory = Join-Path $sessionDirectory $step.trial_id
        Save-Manifest
        if ($SettleSeconds -gt 0) {
            Write-Host ('รอให้สถานะนิ่ง ' + $SettleSeconds + ' วินาที แล้วเริ่มเก็บ')
            Start-Sleep -Seconds $SettleSeconds
        }
        $captureArguments = @{
            Port = $Port; Condition = $step.condition; Mode = $Mode
            DurationSeconds = $DurationSeconds; DeviceLabel = $DeviceLabel
            TrialId = $step.trial_id; PhoneOs = $PhoneOs; CoreVersion = $CoreVersion
            Notes = ('User-confirmed state: ' + $step.title + '; ' + $step.instruction)
            OutputDirectory = $step.directory
        }
        & $captureScript @captureArguments
        $metadataFiles = @(Get-ChildItem -LiteralPath $step.directory -Filter '*.metadata.json' -File)
        if ($metadataFiles.Count -ne 1) { throw 'รอบนี้ไม่มี metadata หนึ่งไฟล์ตามที่คาด' }
        $captureMetadata = [System.IO.File]::ReadAllText($metadataFiles[0].FullName) | ConvertFrom-Json
        if ($captureMetadata.completed -ne $true) { throw 'รอบนี้ยังไม่จบสมบูรณ์ โปรแกรมจะไม่เริ่มรอบถัดไป' }
        $step.status = 'completed'
        Save-Manifest
        Write-Host ('บันทึก ' + $step.title + ' เสร็จแล้ว') -ForegroundColor Green
    }
    if (@($plan | Where-Object { $_.status -ne 'completed' }).Count -eq 0) { $manifest.status = 'completed' }
} catch {
    if ($step -and $step.status -eq 'running') { $step.status = 'failed'; $step.error = $_.Exception.Message }
    $manifest.status = 'failed'
    throw
} finally {
    if ($manifest.status -eq 'running') { $manifest.status = 'interrupted' }
    foreach ($unfinished in @($plan | Where-Object { $_.status -eq 'running' })) { $unfinished.status = 'interrupted' }
    $manifest.ended_utc = [DateTime]::UtcNow.ToString('o')
    Save-Manifest
}

if (@(Get-ChildItem -LiteralPath $sessionDirectory -Recurse -Filter '*.metadata.json' -File).Count) {
    & $summaryScript -SessionDirectory $sessionDirectory
} else {
    Write-Host 'ยังไม่มีรอบที่เก็บข้อมูล เก็บบันทึกสถานะชุดไว้แล้ว'
}
if ($manifest.status -eq 'completed') {
    $zipPath = $sessionDirectory + '.zip'
    Compress-Archive -LiteralPath $sessionDirectory -DestinationPath $zipPath -CompressionLevel Optimal
    Write-Host ''
    Write-Host 'ครบชุดแล้ว ส่งไฟล์ ZIP นี้ได้เลย:' -ForegroundColor Green
    Write-Host $zipPath
} else {
    Write-Host ('จบก่อนครบชุด เก็บผลบางส่วนไว้ที่: ' + $sessionDirectory)
}
