[console]::WindowWidth=30; 
[console]::WindowHeight=2; 
[console]::BufferWidth=[console]::WindowWidth

echo "Restoring all audio ..."

# display and save current defaults 
$currPlay = Get-AudioDevice -List | where {($_.Default -eq "True") -and ($_.Type -eq "Playback")}
$currPlayComm = Get-AudioDevice -List | where {($_.DefaultCommunication -eq "True") -and ($_.Type -eq "Playback")}
$currRec = Get-AudioDevice -List | where {($_.Default -eq "True") -and ($_.Type -eq "Recording")}
$currRecComm = Get-AudioDevice -List | where {($_.DefaultCommunication -eq "True") -and ($_.Type -eq "Recording")}

[System.Collections.ArrayList]$playbackDevices = Get-AudioDevice -List | where type -eq "Playback"
[System.Collections.ArrayList]$recordingDevices = Get-AudioDevice -List | where type -eq "Recording"

#set playbackDevices
for ( $index = 0; $index -lt $playbackDevices.count; $index++ ) {
	
	$maxVol = 100
	$unmute = 0

	Set-AudioDevice -ID $playbackDevices[$index].id | Out-Null
	Set-AudioDevice -PlaybackVolume $maxVol
	Set-AudioDevice -PlaybackMute $unmute
	
}

#set recordingDevices
for ( $index = 0; $index -lt $recordingDevices.count; $index++ ) {
	
	$maxVol = 100
	$unmute = 0

	Set-AudioDevice -ID $recordingDevices[$index].id  | Out-Null
	Set-AudioDevice -RecordingVolume $maxVol
	Set-AudioDevice -RecordingMute $unmute

}

# restore previous audio devices
Set-AudioDevice -ID $currPlay.id -DefaultOnly | Out-Null
Set-AudioDevice -ID $currPlayComm.id -CommunicationOnly | Out-Null

Set-AudioDevice -ID $currRec.id -DefaultOnly | Out-Null
Set-AudioDevice -ID $currRecComm.id -CommunicationOnly| Out-Null