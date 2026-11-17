param(
    [Parameter(Mandatory = $true)][string]$Port,
    [Parameter(Mandatory = $true)]
    [ValidateSet('BACKGROUND', 'BT_OFF', 'BT_ON_HOME', 'BT_SETTINGS', 'SCREEN_LOCKED', 'SHARING_ON', 'ACCESSORY_CONNECTED')]
    [string]$Condition,
    [ValidateSet('passive', 'active')][string]$Mode = 'passive',
    [ValidateRange(10, 600)][int]$DurationSeconds = 300,
    [ValidatePattern('^[A-Za-z0-9_-]{1,40}$')][string]$DeviceLabel = 'HONOR_X7B_5G',
    [ValidatePattern('^[A-Za-z0-9_-]{1,40}$')][string]$TrialId = 'trial01',
    [string]$PhoneOs = 'UNKNOWN',
    [string]$CoreVersion = 'UNKNOWN',
    [string]$Notes = '',
    [string]$OutputDirectory = (Join-Path $PSScriptRoot '../raw_logs/ble_behavior')
)

$ErrorActionPreference = 'Stop'
$sessionId = 'S' + (Get-Date -Format 'yyyyMMdd_HHmmss') + '_' + [guid]::NewGuid().ToString('N').Substring(0, 8)
$baseName = "${sessionId}_${TrialId}_${Condition}_${Mode}"
$null = New-Item -ItemType Directory -Path $OutputDirectory -Force
$outputRoot = (Resolve-Path -LiteralPath $OutputDirectory).Path
$csvPath = Join-Path $outputRoot ($baseName + '.csv')
$rawPath = Join-Path $outputRoot ($baseName + '.serial.log')
$metadataPath = Join-Path $outputRoot ($baseName + '.metadata.json')
$header = 'record_type,schema_version,session_id,trial_id,device_label,condition,scan_mode,timestamp_ms,report_id,mac,addr_type,rssi_dbm,event_type,adv_len,scan_rsp_len,adv_hex,scan_rsp_hex,event,detail,dropped_reports'
$columns = $header.Split(',')
$metadata = [ordered]@{
    schema_version = 1; session_id = $sessionId; trial_id = $TrialId
    device_label = $DeviceLabel; condition = $Condition; scan_mode = $Mode
    requested_duration_seconds = $DurationSeconds; port = $Port; baud_rate = 115200
    board = 'Arduino Nano ESP32'; phone_os = $PhoneOs; core_version = $CoreVersion
    started_utc = [DateTime]::UtcNow.ToString('o'); ended_utc = $null
    completed = $false; observed_reports = 0; last_dropped_reports = 0
    notes = $Notes
}
$serial = $null
$csvWriter = $null
$rawWriter = $null
$started = $false
$ended = $false
$utf8 = New-Object System.Text.UTF8Encoding($false)

try {
    # CreateNew prevents replacement of an existing experiment file.
    $csvStream = [System.IO.File]::Open($csvPath, [System.IO.FileMode]::CreateNew)
    $csvWriter = New-Object System.IO.StreamWriter($csvStream, $utf8)
    $csvWriter.AutoFlush = $true
    $rawStream = [System.IO.File]::Open($rawPath, [System.IO.FileMode]::CreateNew)
    $rawWriter = New-Object System.IO.StreamWriter($rawStream, $utf8)
    $rawWriter.AutoFlush = $true
    $csvWriter.WriteLine($header)
    [System.IO.File]::WriteAllText($metadataPath, ($metadata | ConvertTo-Json), $utf8)

    $serial = New-Object System.IO.Ports.SerialPort($Port, 115200)
    $serial.ReadTimeout = 250
    $serial.WriteTimeout = 2000
    $serial.NewLine = "`n"
    $serial.DtrEnable = $true
    $serial.RtsEnable = $false
    $serial.Open()
    Write-Host "Recording $Condition ($Mode) to $csvPath"

    $clock = [System.Diagnostics.Stopwatch]::StartNew()
    $lastStatusRequest = -1.0
    $startRequested = $false
    $stopRequested = $false
    $scanStartTime = 0.0
    $stopRequestTime = 0.0
    # ReadExisting + our own buffer preserves lines fragmented across read timeouts.
    $buffer = ''
    while (-not $ended) {
        $now = $clock.Elapsed.TotalSeconds
        if (-not $startRequested -and ($now - $lastStatusRequest) -ge 1) {
            $serial.WriteLine('status')
            $lastStatusRequest = $now
        }
        if (-not $started -and $now -gt 20) { throw 'No confirmed scan start within 20 seconds. Check firmware and port.' }
        if ($started -and -not $stopRequested -and ($now - $scanStartTime) -ge $DurationSeconds) {
            $serial.WriteLine('stop')
            $stopRequested = $true
            $stopRequestTime = $now
        }
        if ($stopRequested -and ($now - $stopRequestTime) -gt 15) { throw 'No TRIAL_END after stop. Keep this run marked incomplete.' }

        $chunk = $serial.ReadExisting()
        if ($chunk.Length) {
            $rawWriter.Write($chunk)
            $buffer += $chunk
        }
        while ($buffer.Contains("`n")) {
            $newline = $buffer.IndexOf("`n")
            $line = $buffer.Substring(0, $newline).TrimEnd("`r")
            $buffer = $buffer.Substring($newline + 1)
            if (-not ($line.StartsWith('"REPORT",') -or $line.StartsWith('"EVENT",'))) { continue }
            # Logger escapes quotes and emits no embedded newlines; count CSV fields before parsing.
            $fieldMatches = [regex]::Matches($line, '(?:^|,)("(?:[^"]|"")*"|[^,]*)')
            if ($fieldMatches.Count -ne $columns.Length) { throw 'Malformed logger CSV row; original bytes remain in serial.log.' }
            $row = $line | ConvertFrom-Csv -Header $columns
            if ($row.schema_version -ne '1') { throw 'Unsupported logger schema.' }
            $csvWriter.WriteLine($line)

            if (-not $startRequested -and $row.event -eq 'STATUS') {
                if ($row.detail -notmatch 'trial_open=0;scanning=0;stopping=0;') {
                    throw 'Board already has a trial in progress. Stop it in its original session first.'
                }
                $serial.WriteLine("start $sessionId $TrialId $DeviceLabel $Condition $Mode")
                $startRequested = $true
            }
            if ($startRequested -and $row.event -eq 'BOOT') { throw 'Board reset during trial startup or capture.' }
            if ($row.session_id -ne $sessionId) { continue }
            if ($row.event -eq 'SCAN_STARTED') {
                $started = $true
                $scanStartTime = $clock.Elapsed.TotalSeconds
                Write-Host "Scan confirmed; capturing for $DurationSeconds seconds."
            }
            if ($row.record_type -eq 'REPORT') { $metadata.observed_reports++ }
            if ($row.dropped_reports -ne '') { $metadata.last_dropped_reports = [long]$row.dropped_reports }
            if ($row.event -eq 'TRIAL_END') {
                if ($row.detail -notmatch '^(STOP_CONFIRMED|DURATION_COMPLETE);') { throw "Trial failed: $($row.detail)" }
                if (-not $started) { throw 'Trial ended without confirmed scan start.' }
                $ended = $true
                $metadata.completed = $true
            }
        }
        if ($buffer.Length -gt 16384) { throw 'Serial line exceeds limit. Check firmware and baud rate.' }
        Start-Sleep -Milliseconds 20
    }
} finally {
    if ($serial -and $serial.IsOpen) {
        if ($started -and -not $ended) {
            try { $serial.WriteLine('stop') } catch { }
        }
        $serial.Close()
    }
    if ($serial) { $serial.Dispose() }
    if ($csvWriter) { $csvWriter.Dispose() }
    if ($rawWriter) { $rawWriter.Dispose() }
    $metadata.ended_utc = [DateTime]::UtcNow.ToString('o')
    if (Test-Path -LiteralPath $metadataPath) {
        [System.IO.File]::WriteAllText($metadataPath, ($metadata | ConvertTo-Json), $utf8)
    }
}

Write-Host "Completed. Reports: $($metadata.observed_reports); dropped at logger: $($metadata.last_dropped_reports)"
Write-Host "CSV: $csvPath"
Write-Host "Metadata: $metadataPath"
