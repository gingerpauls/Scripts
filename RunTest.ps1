
Function RunAndTime {
    Param(
        [string]$script
    )
        [console]::WindowWidth=80; 
        [console]::WindowHeight=86; 
        [console]::BufferWidth=[console]::WindowWidth
        $StopWatch = [system.diagnostics.stopwatch]::startNew()
        
        invoke-expression $script
    
        Write-Host -NoNewline $StopWatch.ElapsedMilliseconds
        Write-Host " ms `n"
    }
    
    RunAndTime "D:\Scripts\WrongAudio.ps1"
    
    RunAndTime "D:\Scripts\RestoreGoXLRAudio.ps1"
    
    RunAndTime "D:\Scripts\RestoreAllAudio.ps1"
    
    RunAndTime "D:\Scripts\ReportAudioDevices.ps1"

    RunAndTime "D:\Scripts\CAudioDevices\build\audio.exe"