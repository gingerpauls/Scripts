﻿/*
Macros.ahk

todo
	I want to be able to use the capslock button to toggle between three window sizes (small, medium, large).

	Add custom sizes 
		thin & tall
		short & fat
		make as tall as monitor, same width

		git test
*/

;#SingleInstance Force
SetTitleMatchMode, 2
DetectHiddenWindows, On
MonitorSizeIndex = 0

; Desk
^!1::
	Run, D:\Programming\Programs\multimonitortool-x64\MultiMonitorTool.exe /LoadConfig "D:\Programming\Programs\multimonitortool-x64\Monitor Config\Desk.cfg"
	Run, D:\Programming\Programs\soundvolumeview-x64\SoundVolumeView.exe /LoadProfile "D:\Programming\Programs\soundvolumeview-x64\Sound Profiles\Desk.spr"
	Process, Close, REAPER.exe
return

; TV
^!2::
	Run, D:\Programming\Programs\multimonitortool-x64\MultiMonitorTool.exe /LoadConfig "D:\Programming\Programs\multimonitortool-x64\Monitor Config\TV.cfg"
	Run, D:\Programming\Programs\soundvolumeview-x64\SoundVolumeView.exe /LoadProfile "D:\Programming\Programs\soundvolumeview-x64\Sound Profiles\TV.spr"
	Process, Close, REAPER.exe
return

; Stream
^!+1::
	Run, D:\Programming\Programs\multimonitortool-x64\MultiMonitorTool.exe /LoadConfig "D:\Programming\Programs\multimonitortool-x64\Monitor Config\Stream.cfg"
	Run, D:\Programming\Programs\soundvolumeview-x64\SoundVolumeView.exe /LoadProfile "D:\Programming\Programs\soundvolumeview-x64\Sound Profiles\Desk.spr"
return

; TV / CABLE OUTPUT
^!+2::
	Run, "C:\Program Files\REAPER (x64)\reaper.exe"
	Run, D:\Programming\Programs\soundvolumeview-x64\SoundVolumeView.exe /LoadProfile "D:\Programming\Programs\soundvolumeview-x64\Sound Profiles\Reaper.spr"
return

; All Screens On - TV Speakers
^!3::
	Run, D:\Programming\Programs\multimonitortool-x64\MultiMonitorTool.exe /LoadConfig "D:\Programming\Programs\multimonitortool-x64\Monitor Config\All.cfg"
	Run, D:\Programming\Programs\soundvolumeview-x64\SoundVolumeView.exe /LoadProfile "D:\Programming\Programs\soundvolumeview-x64\Sound Profiles\All.spr"
return

; Desk with REAPER
^!+3::
	Run, "C:\Program Files\REAPER (x64)\reaper.exe"
	;Run, D:\Programming\Programs\multimonitortool-x64\MultiMonitorTool.exe /LoadConfig "D:\Programming\Programs\multimonitortool-x64\Monitor Config\Desk.cfg"
	Run, D:\Programming\Programs\soundvolumeview-x64\SoundVolumeView.exe /LoadProfile "D:\Programming\Programs\soundvolumeview-x64\Sound Profiles\DeskReaper.spr"
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
	if WinActive("Settings")
			WinClose
		else 
		{
			Run ms-settings:display
			sleep 0250
			WinRestore, Settings
			sleep 0060
			WinMove, Settings, , X, Y, 740, 920
		}
return

; Mouse Settings
^!+m::
	if WinActive("Settings")
			WinClose
		else 
		{
			Run ms-settings:mousetouchpad
			sleep 0250
			WinRestore, Settings
			sleep 0060
			WinMove, Settings, , X, Y, 740, 920
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
		WinMove, A, ,X, Y, 640, 1408
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
	Run, D:\Scripts\Macros.ahk
return

; Compile Macros.exe and move to common startup
^!+c::
	Run, C:\Program Files\AutoHotkey\Compiler\Ahk2Exe.exe /in "D:\Programming\Scripts\Macros.ahk" /out "C:\Users\hutch\AppData\Roaming\Microsoft\Windows\Start Menu\Programs\Startup\Macros.exe"
return

