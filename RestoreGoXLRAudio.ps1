[console]::WindowWidth=30; 
[console]::WindowHeight=2; 
[console]::BufferWidth=[console]::WindowWidth

echo "Fixing GoXLR Audio..."

[System.Collections.ArrayList]$audioDevices = Get-AudioDevice -List | where name -like "*TC-Helicon*"

$maxVol = 100
$unmute = 0

for ( $index = 0; $index -lt $audioDevices.count; $index++ ) {

	Set-AudioDevice -ID $audioDevices[$index].id | Out-Null
	Set-AudioDevice -PlaybackVolume $maxVol
	Set-AudioDevice -PlaybackMute $unmute
	Set-AudioDevice -RecordingVolume $maxVol
	Set-AudioDevice -RecordingMute $unmute
	
}

Get-AudioDevice -List | where Type -like "Playback" | where name -like "*System*TC-Helicon*"  | Set-AudioDevice -Verbose -DefaultOnly | Out-Null
Get-AudioDevice -List | where Type -like "Playback" | where name -like "*Chat*TC-Helicon*"  | Set-AudioDevice -Verbose -CommunicationOnly | Out-Null
Get-AudioDevice -List | where Type -like "Recording" | where name -like "*Mic*TC-Helicon*"  | Set-AudioDevice -Verbose | Out-Null