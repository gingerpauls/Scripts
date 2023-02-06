Function RunAndTime {
Param(
    [string]$script
)
    $StopWatch = [system.diagnostics.stopwatch]::startNew()
    
    invoke-expression $script

    $StopWatch.Elapsed
}

RunAndTime "D:\Scripts\WrongAudio.ps1"

RunAndTime "D:\Scripts\RestoreGoXLRAudio.ps1"

RunAndTime "D:\Scripts\ReportAudioDevices.ps1"

