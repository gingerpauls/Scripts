/*
Macros.ahk
*/

; Ctrl: ^, Alt: !, Shift: +

;#SingleInstance Force
SetTitleMatchMode, 2
DetectHiddenWindows, On
DetectHiddenText, On
MonitorSizeIndex = 0

; Desk
^!1::
    Run, D:\Programming\Tools\NirSoft\multimonitortool-x64\MultiMonitorTool.exe /LoadConfig "D:\Programming\Tools\NirSoft\multimonitortool-x64\Monitor Config\Desk.cfg"
    ;Run, D:\Programming\Tools\NirSoft\soundvolumeview-x64\SoundVolumeView.exe /LoadProfile "D:\Programming\Tools\NirSoft\soundvolumeview-x64\Sound Profiles\Desk.spr"
    ;Process, Close, REAPER.exe
return
; TV
^!2::
    Run, D:\Programming\Tools\NirSoft\multimonitortool-x64\MultiMonitorTool.exe /LoadConfig "D:\Programming\Tools\NirSoft\multimonitortool-x64\Monitor Config\TV.cfg"
    ;Run, D:\Programming\Tools\NirSoft\soundvolumeview-x64\SoundVolumeView.exe /LoadProfile "D:\Programming\Tools\NirSoft\soundvolumeview-x64\Sound Profiles\TV.spr"
    ;Process, Close, REAPER.exe
return
; All Screens On - TV Speakers
^!3::
    Run, D:\Programming\Tools\NirSoft\multimonitortool-x64\MultiMonitorTool.exe /LoadConfig "D:\Programming\Tools\NirSoft\multimonitortool-x64\Monitor Config\All.cfg"
    ;Run, D:\Programming\Tools\NirSoft\soundvolumeview-x64\SoundVolumeView.exe /LoadProfile "D:\Programming\Tools\NirSoft\soundvolumeview-x64\Sound Profiles\All.spr"
return
; Stream
^!+1::
    Run, D:\Programming\Tools\NirSoft\multimonitortool-x64\MultiMonitorTool.exe /LoadConfig "D:\Programming\Tools\NirSoft\multimonitortool-x64\Monitor Config\Stream.cfg"
    ;un, D:\Programming\Tools\NirSoft\soundvolumeview-x64\SoundVolumeView.exe /LoadProfile "D:\Programming\Tools\NirSoft\soundvolumeview-x64\Sound Profiles\Desk.spr"
return
; TV with REAPER
^!+2::
    Run, D:\Programming\Tools\NirSoft\multimonitortool-x64\MultiMonitorTool.exe /LoadConfig "D:\Programming\Tools\NirSoft\multimonitortool-x64\Monitor Config\TV.cfg"
    Run, "C:\Program Files\REAPER (x64)\reaper.exe"
    ;Run, D:\Programming\Tools\NirSoft\soundvolumeview-x64\SoundVolumeView.exe /LoadProfile "D:\Programming\Tools\NirSoft\soundvolumeview-x64\Sound Profiles\Reaper.spr"
return
; Desk with REAPER
^!+3::
    ;Run, "C:\Program Files\REAPER (x64)\reaper.exe"
    Run, D:\Programming\NirSoft\soundvolumeview-x64\SoundVolumeView.exe /LoadProfile "D:\Programming\NirSoft\soundvolumeview-x64\Sound Profiles\DeskReaper.spr"
    ;Process, Close, REAPER.exe
return

; Legacy Sound
^!+s::
    if WinActive("Sound")
        WinClose
    else Run Mmsys.cpl
        return

; Display Settings
^!+d::
    if WinActive("Settings"){
        WinClose
    }
    else 
    {
        Run ms-settings:display
        sleep 0300
        WinMove, Settings, , X, 0, 727, 1399
    }
return


; Mixer Settings
^!+x::
    if WinActive("Settings"){
        WinClose
    }
    else 
    {
        Run ms-settings:apps-volume
        sleep 0300
        WinMove, Settings, , X, 0, 727, 1399
    }
return


; Mouse Settings
^!+m::
    if WinActive("Settings"){
        WinClose
    }
    else 
    {
        Run ms-settings:mousetouchpad
        sleep 0300
        WinMove, Settings, , X, 0, 727, 1399
    }
return

; Resize active window to be small, medium, large
^!+z::
    StoreOriginalWinPos(X, Y, W, H, A)
    if MonitorSizeIndex >= 3
        MonitorSizeIndex = 0
    WinMoveBasedOnIndex(MonitorSizeIndex)
    MonitorSizeIndex++
return
WinMoveBasedOnIndex(MonitorSizeIndex) 
{
    if MonitorSizeIndex = 0
        WinMove, A, ,X, Y, 640, 355
    if MonitorSizeIndex = 1
        WinMove, A, ,X, Y, 727, 1399
    if MonitorSizeIndex = 2
        WinMove, A, ,X, Y, 1280, 720
return
}
StoreOriginalWinPos(X, Y, W, H, A)
{
    WinGetPos, X, Y, W, H, A
return
}

;Always on top, Alt+t
^!+t::
    WinSet, AlwaysOnTop, Toggle, A
return

; GetWinPos
^!0::
    WinGetPos, X, Y, W, H, A
    WinGetActiveStats, Title, Width, Height, X, Y
    MsgBox, The active window "%Title%" is positioned at %X%`,%Y%, and %Width% wide`, %Height% tall`.
return

; Run Macros.ahk 
^!+r::
    Process, Close, Macros.exe
    Run, D:\Programming\Scripts\Macros.ahk
return

; Compile Macros.exe and move to common startup
^!+c::
    Run, C:\Program Files\AutoHotkey\Compiler\Ahk2Exe.exe /in "D:\Programming\Projects\Scripts\Macros.ahk" /out "C:\Users\hutch\AppData\Roaming\Microsoft\Windows\Start Menu\Programs\Startup\Macros.exe"
return

; Run Dev apps
^!v::
    Run, "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\devenv.exe"
    WinMove, Microsoft Visual Studio, , X, 0, 727, 1399
    Run, "C:\Program Files\Google\Chrome\Application\chrome.exe"
    Run, "C:\Users\hutch\AppData\Local\Programs\Microsoft VS Code\Code.exe"
    WinMove, code.exe, , X, 0, 727, 1399
    Run, "C:\Program Files\Sublime Merge\sublime_merge.exe"
return
