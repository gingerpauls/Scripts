#Sets random mute, and volume to all audio devices (playback and recording)

# [console]::WindowWidth=60; 
# [console]::WindowHeight=50; 
# [console]::BufferWidth=[console]::WindowWidth

#separate into playback and recording lists
[System.Collections.ArrayList]$audioDevices = Get-AudioDevice -List

echo "Randomizing all enabled audio devices..."
#set audioDevices
for ( $index = 0; $index -lt $audioDevices.count; $index++ ) {
	$randVol = Get-Random -Maximum 100
	$randMute = Get-Random -Maximum 2

	Set-AudioDevice -ID $audioDevices[$index].id | Out-Null
	Set-AudioDevice -PlaybackVolume $randVol
	Set-AudioDevice -PlaybackMute $randMute
	Set-AudioDevice -RecordingVolume $randVol
	Set-AudioDevice -RecordingMute $randMute

}

#set random playback and recording devices
$x = $audioDevices.count
$randDev = Get-Random -Maximum $x
Set-AudioDevice -ID $audioDevices[$randDev].id -DefaultOnly | Out-Null
$randDev = Get-Random -Maximum $x
Set-AudioDevice -ID $audioDevices[$randDev].id -CommunicationOnly | Out-Null