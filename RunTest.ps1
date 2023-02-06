$StopWatch = [system.diagnostics.stopwatch]::startNew()
invoke-expression -command D:\Scripts\WrongAudio.ps1
$StopWatch.Elapsed
$StopWatch = [system.diagnostics.stopwatch]::startNew()
invoke-expression -command D:\Scripts\RestoreGoXLRAudio.ps1
$StopWatch.Elapsed
$StopWatch = [system.diagnostics.stopwatch]::startNew()
invoke-expression  "D:\Scripts\ReportAudioDevices.ps1"
$StopWatch.Elapsed