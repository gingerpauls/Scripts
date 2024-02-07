
Function RunAndTime {
    Param(
        [string]$script
    )
        #[console]::WindowWidth=80; 
        #[console]::WindowHeight=86; 
        [console]::BufferWidth=[console]::WindowWidth
        $StopWatch = [system.diagnostics.stopwatch]::startNew()
        
        invoke-expression $script
    
        Write-Host -NoNewline $StopWatch.ElapsedMilliseconds
        Write-Host " ms `n"
    }
    
    #RunAndTime "D:\Scripts\WrongAudio.ps1"
    Write-Host "Randomizing all audio..."
    RunAndTime "D:\Scripts\CAudioDevices\build\audio.exe -r"

    #RunAndTime "D:\Scripts\RestoreGoXLRAudio.ps1"
    Write-Host "Setting TC-Helicon defualt deviecs. Unmuting and max volume for all..."
    RunAndTime "D:\Scripts\CAudioDevices\build\audio.exe -TC"

    #RunAndTime "D:\Scripts\RestoreAllAudio.ps1"
    Write-Host "Unmuting and max volume all devices..."
    RunAndTime "D:\Scripts\CAudioDevices\build\audio.exe -u *"
    
    #RunAndTime "D:\Scripts\ReportAudioDevices.ps1"
    Write-Host "Listing all devices..."
    RunAndTime "D:\Scripts\CAudioDevices\build\audio.exe -l"
