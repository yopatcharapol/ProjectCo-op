param([Parameter(Mandatory = $true)][string]$SessionDirectory)

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $SessionDirectory).Path

function Read-AdFields([string]$HexData) {
    if ($HexData -notmatch '^(?:[0-9A-Fa-f]{2})*$') { throw 'Payload is not hexadecimal bytes' }
    $bytes = @()
    for ($i = 0; $i -lt $HexData.Length; $i += 2) { $bytes += [Convert]::ToByte($HexData.Substring($i, 2), 16) }
    $names = @(); $companies = @(); $services = @(); $txPowers = @(); $errors = 0
    $offset = 0
    while ($offset -lt $bytes.Count) {
        $length = [int]$bytes[$offset]
        if ($length -eq 0) { break }
        if ($offset + $length -ge $bytes.Count) { $errors++; break }
        $type = $bytes[$offset + 1]
        $dataLength = $length - 1
        $dataStart = $offset + 2
        if (($type -eq 8 -or $type -eq 9) -and $dataLength -gt 0) {
            $nameBytes = [byte[]]$bytes[$dataStart..($dataStart + $dataLength - 1)]
            $names += [Text.Encoding]::UTF8.GetString($nameBytes).Replace("`r", ' ').Replace("`n", ' ')
        }
        if ($type -eq 255 -and $dataLength -ge 2) {
            $companies += ('0x{0:X4}' -f ($bytes[$dataStart] + 256 * $bytes[$dataStart + 1]))
        }
        if ($type -eq 22 -and $dataLength -ge 2) {
            $services += ('0x{0:X4}' -f ($bytes[$dataStart] + 256 * $bytes[$dataStart + 1]))
        }
        if ($type -eq 2 -or $type -eq 3) {
            if ($dataLength % 2) { $errors++ }
            for ($j = 0; $j + 1 -lt $dataLength; $j += 2) {
                $services += ('0x{0:X4}' -f ($bytes[$dataStart + $j] + 256 * $bytes[$dataStart + $j + 1]))
            }
        }
        if ($type -eq 10 -and $dataLength -eq 1) {
            $txPower = [int]$bytes[$dataStart]
            if ($txPower -ge 128) { $txPower -= 256 }
            $txPowers += $txPower
        }
        $offset += $length + 1
    }
    [pscustomobject]@{names=$names; companies=$companies; services=$services; tx_powers=$txPowers; errors=$errors}
}

$trialSummary = @(); $deviceSummary = @()
foreach ($metadataFile in @(Get-ChildItem -LiteralPath $root -Recurse -Filter '*.metadata.json' -File | Sort-Object FullName)) {
    $metadata = [System.IO.File]::ReadAllText($metadataFile.FullName) | ConvertFrom-Json
    $captureId = if ($metadata.capture_id) { $metadata.capture_id } else { $metadata.trial_id }
    $stateLabel = if ($metadata.state_label) { $metadata.state_label } else { $metadata.condition }
    $csvPath = $metadataFile.FullName.Substring(0, $metadataFile.FullName.Length - '.metadata.json'.Length) + '.csv'
    if (-not (Test-Path -LiteralPath $csvPath)) { throw "CSV missing for $($metadataFile.Name)" }
    $reports = @(Import-Csv -LiteralPath $csvPath -Encoding UTF8 | Where-Object {
        $_.record_type -eq 'REPORT' -and $_.session_id -eq $metadata.session_id -and $_.trial_id -eq $metadata.trial_id
    })
    $decoded = @(); $invalidRows = 0; $adErrors = 0
    foreach ($row in $reports) {
        $rssi = 0; $timestamp = 0L; $advLength = 0; $responseLength = 0
        if (-not [int]::TryParse($row.rssi_dbm, [ref]$rssi) -or
            -not [long]::TryParse($row.timestamp_ms, [ref]$timestamp) -or
            -not [int]::TryParse($row.adv_len, [ref]$advLength) -or
            -not [int]::TryParse($row.scan_rsp_len, [ref]$responseLength) -or
            $advLength -lt 0 -or $responseLength -lt 0 -or
            $row.adv_hex.Length -ne 2 * $advLength -or $row.scan_rsp_hex.Length -ne 2 * $responseLength -or
            $row.adv_hex -notmatch '^(?:[0-9A-Fa-f]{2})*$' -or $row.scan_rsp_hex -notmatch '^(?:[0-9A-Fa-f]{2})*$') {
            $invalidRows++; continue
        }
        $adv = Read-AdFields $row.adv_hex
        $response = Read-AdFields $row.scan_rsp_hex
        $adErrors += $adv.errors + $response.errors
        $decoded += [pscustomobject]@{
            mac=$row.mac; rssi=$rssi; timestamp=$timestamp
            names=@($adv.names) + @($response.names)
            companies=@($adv.companies) + @($response.companies)
            services=@($adv.services) + @($response.services)
            tx_powers=@($adv.tx_powers) + @($response.tx_powers)
        }
    }
    $trialSummary += [pscustomobject][ordered]@{
        capture_id=$captureId; state_label=$stateLabel
        trial_id=$metadata.trial_id; condition=$metadata.condition; completed=$metadata.completed
        duration_requested_s=$metadata.requested_duration_seconds; mode=$metadata.scan_mode
        report_rows=$reports.Count; valid_rows=$decoded.Count; invalid_rows=$invalidRows
        ad_parse_errors=$adErrors; address_count=@($decoded.mac | Sort-Object -Unique).Count
        fef3_report_rows=@($decoded | Where-Object { $_.services -contains '0xFEF3' }).Count
        dropped_reports=$metadata.last_dropped_reports; notes=$metadata.notes
    }
    foreach ($group in @($decoded | Group-Object mac)) {
        $rssiStats = $group.Group.rssi | Measure-Object -Average -Minimum -Maximum
        $times = @($group.Group.timestamp | Sort-Object)
        $deviceSummary += [pscustomobject][ordered]@{
            capture_id=$captureId; state_label=$stateLabel
            trial_id=$metadata.trial_id; condition=$metadata.condition; completed=$metadata.completed
            mac=$group.Name; reports=$group.Count
            rssi_min_dbm=$rssiStats.Minimum; rssi_max_dbm=$rssiStats.Maximum
            rssi_mean_dbm=[math]::Round($rssiStats.Average, 2)
            first_seen_ms=$times[0]; last_seen_ms=$times[-1]
            name_hints=(@($group.Group.names | Sort-Object -Unique) -join ';')
            manufacturer_ids=(@($group.Group.companies | Sort-Object -Unique) -join ';')
            service_uuids16=(@($group.Group.services | Sort-Object -Unique) -join ';')
            advertised_tx_power_dbm=(@($group.Group.tx_powers | Sort-Object -Unique) -join ';')
        }
    }
}
if ($trialSummary.Count -eq 0) { throw 'No capture metadata found in this session.' }
$trialSummary | Export-Csv -LiteralPath (Join-Path $root 'summary.csv') -NoTypeInformation -Encoding UTF8
if ($deviceSummary.Count) {
    $deviceSummary | Export-Csv -LiteralPath (Join-Path $root 'addresses.csv') -NoTypeInformation -Encoding UTF8
} else {
    Set-Content -LiteralPath (Join-Path $root 'addresses.csv') -Encoding UTF8 -Value 'capture_id,state_label,trial_id,condition,completed,mac,reports,rssi_min_dbm,rssi_max_dbm,rssi_mean_dbm,first_seen_ms,last_seen_ms,name_hints,manufacturer_ids,service_uuids16,advertised_tx_power_dbm'
}
$trialSummary | Select-Object trial_id,condition,completed,report_rows,fef3_report_rows,invalid_rows,dropped_reports | Format-Table -AutoSize | Out-Host
Write-Host ('Summary: ' + (Join-Path $root 'summary.csv'))
Write-Host ('Address details: ' + (Join-Path $root 'addresses.csv'))
