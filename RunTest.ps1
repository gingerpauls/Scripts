
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
    ## randomize all audio
    Write-Host "Randomizing..."
    RunAndTime "D:\Scripts\CAudioDevices\build\audio.exe -r"

    #RunAndTime "D:\Scripts\RestoreGoXLRAudio.ps1"
    ## unmute, max volume, and set TC-Helicon devices to default
    Write-Host "Setting default devices..."
    RunAndTime "D:\Scripts\CAudioDevices\build\audio.exe -TC"

    #RunAndTime "D:\Scripts\RestoreAllAudio.ps1"
    ## unmute, max volume all devices
    Write-Host "Unmuting and maxing all devices..."
    RunAndTime "D:\Scripts\CAudioDevices\build\audio.exe -u *"
    
    #RunAndTime "D:\Scripts\ReportAudioDevices.ps1"
    ## list all devices
    Write-Host "Listing all devices..."
    RunAndTime "D:\Scripts\CAudioDevices\build\audio.exe -l"
