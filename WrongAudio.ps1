#Sets random mute, and volume to all audio devices (playback and recording)
[System.Collections.ArrayList]$playbackDevices = Get-AudioDevice -List | where {($_.Type -eq "Playback")}
[System.Collections.ArrayList]$recordingDevices = Get-AudioDevice -List | where {($_.Type -eq "Recording")}


#set playbackDevices
for ( $index = 0; $index -lt $playbackDevices.count; $index++ ) {
	$randVol = Get-Random -Maximum 100
	$randMute = Get-Random -Maximum 2

	Set-AudioDevice -ID $playbackDevices[$index].id | Out-Null
	Set-AudioDevice -PlaybackVolume $randVol
	Set-AudioDevice -PlaybackMute $randMute

}

#set recordingDevices
for ( $index = 0; $index -lt $recordingDevices.count; $index++ ) {
	
	$randVol = Get-Random -Maximum 100
	$randMute = Get-Random -Maximum 1

	Set-AudioDevice -ID $recordingDevices[$index].id  | Out-Null
	Set-AudioDevice -RecordingVolume $randVol
	Set-AudioDevice -RecordingMute $randMute

}

#set random playback and recording devices
$x = $playbackDevices.count
$y = $recordingDevices.count

$randPlayDev = Get-Random -Maximum $x
$randRecDev = Get-Random -Maximum $y

Set-AudioDevice -ID $playbackDevices[$randPlayDev].id -DefaultOnly| Out-Null
Set-AudioDevice -ID $recordingDevices[$randRecDev].id -DefaultOnly | Out-Null

$randPlayDev = Get-Random -Maximum $x
$randRecDev = Get-Random -Maximum $y

Set-AudioDevice -ID $recordingDevices[$randRecDev].id -CommunicationOnly | Out-Null
Set-AudioDevice -ID $playbackDevices[$randPlayDev].id -CommunicationOnly| Out-Null