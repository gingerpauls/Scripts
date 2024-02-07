
# [console]::WindowWidth=60; 
# [console]::WindowHeight=50; 
# [console]::BufferWidth=[console]::WindowWidth

echo "Restoring all audio devices ..."

[System.Collections.ArrayList]$audioDevices = Get-AudioDevice -List

# display and save current defaults 
$currPlay = $audioDevices | where {($_.Default -eq "True") -and ($_.Type -eq "Playback")}
$currPlayComm = $audioDevices | where {($_.DefaultCommunication -eq "True") -and ($_.Type -eq "Playback")}
$currRec = $audioDevices | where {($_.Default -eq "True") -and ($_.Type -eq "Recording")}
$currRecComm = $audioDevices | where {($_.DefaultCommunication -eq "True") -and ($_.Type -eq "Recording")}

#set audioDevices
for ( $index = 0; $index -lt $audioDevices.count; $index++ ) {
	$maxVol = 100
	$unmute = 0

	Set-AudioDevice -ID $audioDevices[$index].id | Out-Null
	Set-AudioDevice -PlaybackVolume $maxVol
	Set-AudioDevice -PlaybackMute $unmute
	Set-AudioDevice -RecordingVolume $maxVol
	Set-AudioDevice -RecordingMute $unmute
	
}

# restore previous audio devices
Set-AudioDevice -ID $currPlay.id -DefaultOnly | Out-Null
Set-AudioDevice -ID $currPlayComm.id -CommunicationOnly | Out-Null

Set-AudioDevice -ID $currRec.id -DefaultOnly | Out-Null
Set-AudioDevice -ID $currRecComm.id -CommunicationOnly| Out-Null