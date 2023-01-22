$StopWatch = [system.diagnostics.stopwatch]::startNew()
#Sets random mute, and volume to all audio devices (playback and recording)

[System.Collections.ArrayList]$playbackDevices = Get-AudioDevice -List | where {($_.Type -eq "Playback")}
[System.Collections.ArrayList]$recordingDevices = Get-AudioDevice -List | where {($_.Type -eq "Recording")}

#open Windows Sounds
control mmsys.cpl sounds

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

#get playbackDevices
echo ""
echo "Playback Devices"
echo ""

for ( $index = 0; $index -lt $playbackDevices.count; $index++ ) {
	
	Set-AudioDevice -ID $playbackDevices[$index].id | Out-Null
	echo $playbackDevices[$index].name
	
	Write-Host -NoNewline "Vol: "
	Get-AudioDevice -PlaybackVolume | Write-Host -NoNewline
	Write-Host -NoNewline " / Mute: "
	Get-AudioDevice -PlaybackMute | Write-Host -NoNewline
	Write-Host -NoNewline " / ComVol: "
	Get-AudioDevice -PlaybackCommunicationVolume | Write-Host -NoNewline
	Write-Host -NoNewline " / ComMute: "
	Get-AudioDevice -PlaybackCommunicationMute | Write-Host

}

#get recordingDevices
echo ""
echo "Recording Devices:"
echo ""

for ( $index = 0; $index -lt $recordingDevices.count; $index++ ) {
	
	Set-AudioDevice -ID $recordingDevices[$index].id  | Out-Null
	echo $recordingDevices[$index].name
	
	Write-Host -NoNewline "Vol: "
	Get-AudioDevice -RecordingVolume | Write-Host -NoNewline
	Write-Host -NoNewline " / Mute: "
	Get-AudioDevice -RecordingMute | Write-Host -NoNewline
	Write-Host -NoNewline " / ComVol: "
	Get-AudioDevice -RecordingCommunicationVolume | Write-Host -NoNewline
	Write-Host -NoNewline " / ComMute: "
	Get-AudioDevice -RecordingCommunicationMute | Write-Host

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

echo "" 
echo "" 
echo "" 

# display and save current defaults 
$currPlay = Get-AudioDevice -List | where {($_.Default -eq "True") -and ($_.Type -eq "Playback")}
echo "Current Default Playback Device: " $currPlay.name
echo ""
$currPlayComm = Get-AudioDevice -List | where {($_.DefaultCommunication -eq "True") -and ($_.Type -eq "Playback")}
echo "Current Default Playback Commucation Device: " $currPlayComm.name 
echo ""
$currRec = Get-AudioDevice -List | where {($_.Default -eq "True") -and ($_.Type -eq "Recording")}
echo "Current Default Recording Device: " $currRec.name 
echo ""
$currRecComm = Get-AudioDevice -List | where {($_.DefaultCommunication -eq "True") -and ($_.Type -eq "Recording")}
echo "Current Default Recording Communication Device: " $currRecComm.name 
echo ""


echo "" 
echo "" 
echo "" 
$StopWatch.Elapsed