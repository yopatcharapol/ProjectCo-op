param(
    [Parameter(Mandatory = $true)][string]$SummaryCsv,
    [Parameter(Mandatory = $true)][string]$OutputPng
)

$ErrorActionPreference = 'Stop'
Add-Type -AssemblyName System.Windows.Forms.DataVisualization
$rows = @(Import-Csv -LiteralPath $SummaryCsv -Encoding UTF8)
if ($rows.Count -ne 5 -or @($rows | Where-Object completed -ne 'True').Count) { throw 'Expected five complete pilot captures.' }
$labels = @{
    BT_OFF='Bluetooth off';BT_ON_HOME="Bluetooth on`nHome screen"
    QUICK_SHARE_SCREEN_ON="Quick Share`nScreen on";SCREEN_LOCKED="Screen`nlocked"
    QUICK_SHARE_UNLOCKED="Unlock`nQuick Share"
}
$chart = New-Object System.Windows.Forms.DataVisualization.Charting.Chart
try {
    $chart.Width=1100; $chart.Height=620; $chart.BackColor=[Drawing.Color]::White
    $chart.AntiAliasing='All'
    $area=New-Object System.Windows.Forms.DataVisualization.Charting.ChartArea('Pilot')
    $area.AxisX.Interval=1
    $area.AxisX.MajorGrid.Enabled=$false
    $area.AxisX.LabelStyle.Font=New-Object Drawing.Font('Segoe UI',11)
    $area.AxisY.Title='Received reports in one 30-second capture'
    $area.AxisY.TitleFont=New-Object Drawing.Font('Segoe UI',11)
    $area.AxisY.Minimum=0; $area.AxisY.Maximum=700; $area.AxisY.Interval=100
    $area.AxisY.MajorGrid.LineColor=[Drawing.Color]::FromArgb(230,233,238)
    $chart.ChartAreas.Add($area)
    $legend=New-Object System.Windows.Forms.DataVisualization.Charting.Legend('Legend')
    $legend.Docking='Bottom'; $legend.Alignment='Center'
    $legend.Font=New-Object Drawing.Font('Segoe UI',11)
    $chart.Legends.Add($legend)
    foreach ($seriesDefinition in @(
        @{name='All received reports';field='report_rows';color=[Drawing.Color]::FromArgb(50,105,165)},
        @{name='Reports containing service UUID 0xFEF3';field='fef3_report_rows';color=[Drawing.Color]::FromArgb(225,140,35)}
    )) {
        $series=New-Object System.Windows.Forms.DataVisualization.Charting.Series($seriesDefinition.name)
        $series.ChartType='Column'; $series.Color=$seriesDefinition.color
        $series.IsValueShownAsLabel=$true; $series.Font=New-Object Drawing.Font('Segoe UI',11)
        foreach ($row in $rows) {
            $index=$series.Points.AddXY($labels[$row.state_label],[int]$row.($seriesDefinition.field))
            $series.Points[$index].ToolTip=$row.capture_id
        }
        $chart.Series.Add($series)
    }
    $title=$chart.Titles.Add('BLE pilot 01: observed reports across five phone states')
    $title.Font=New-Object Drawing.Font('Segoe UI',16,[Drawing.FontStyle]::Bold)
    $note=$chart.Titles.Add('One capture per state. A service UUID does not identify a physical phone.')
    $note.Docking='Bottom'; $note.Font=New-Object Drawing.Font('Segoe UI',10)
    $outputPath=[IO.Path]::GetFullPath($OutputPng)
    $parent=[IO.Path]::GetDirectoryName($outputPath)
    if (-not (Test-Path -LiteralPath $parent)) { $null=New-Item -ItemType Directory -Path $parent }
    $chart.SaveImage($outputPath,[Windows.Forms.DataVisualization.Charting.ChartImageFormat]::Png)
    Write-Host $outputPath
} finally { $chart.Dispose() }
