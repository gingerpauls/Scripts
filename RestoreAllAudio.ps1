cls
$StopWatch = [system.diagnostics.stopwatch]::startNew()
#install AudioDeviceCmdlets
#Install-Module -Name AudioDeviceCmdlets -Scope CurrentUser

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

[System.Collections.ArrayList]$playbackDevices = Get-AudioDevice -List | where {($_.Type -eq "Playback")}
[System.Collections.ArrayList]$recordingDevices = Get-AudioDevice -List | where {($_.Type -eq "Recording")}

#open Windows Sounds
control mmsys.cpl sounds

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

# restore previous audio devices
Set-AudioDevice -ID $currPlay.id -DefaultOnly | Out-Null
Set-AudioDevice -ID $currPlayComm.id -CommunicationOnly | Out-Null

Set-AudioDevice -ID $currRec.id -DefaultOnly | Out-Null
Set-AudioDevice -ID $currRecComm.id -CommunicationOnly| Out-Null

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