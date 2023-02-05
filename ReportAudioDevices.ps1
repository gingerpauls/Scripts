cls
[console]::WindowWidth=60; 
[console]::WindowHeight=50; 
[console]::BufferWidth=[console]::WindowWidth

#Sets random mute, and volume to all audio devices (playback and recording)
[System.Collections.ArrayList]$playbackDevices = Get-AudioDevice -List | where {($_.Type -eq "Playback")}
[System.Collections.ArrayList]$recordingDevices = Get-AudioDevice -List | where {($_.Type -eq "Recording")}

#open Windows Sounds
#control mmsys.cpl sounds

# save current defaults 
$currPlay = Get-AudioDevice -List | where {($_.Default -eq "True") -and ($_.Type -eq "Playback")}
$currPlayComm = Get-AudioDevice -List | where {($_.DefaultCommunication -eq "True") -and ($_.Type -eq "Playback")}
$currRec = Get-AudioDevice -List | where {($_.Default -eq "True") -and ($_.Type -eq "Recording")}
$currRecComm = Get-AudioDevice -List | where {($_.DefaultCommunication -eq "True") -and ($_.Type -eq "Recording")}

#get playbackDevices
echo "Current Playback Devices"
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
echo "Current Recording Devices:"
echo ""

for ( $index = 0; $index -lt $recordingDevices.count; $index++ ) {
	
	Set-AudioDevice -ID $recordingDevices[$index].id | Out-Null
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
# display and save current defaults 
$currPlay = Get-AudioDevice -List | where {($_.Default -eq "True") -and ($_.Type -eq "Playback")}
$currPlayComm = Get-AudioDevice -List | where {($_.DefaultCommunication -eq "True") -and ($_.Type -eq "Playback")}
$currRec = Get-AudioDevice -List | where {($_.Default -eq "True") -and ($_.Type -eq "Recording")}
$currRecComm = Get-AudioDevice -List | where {($_.DefaultCommunication -eq "True") -and ($_.Type -eq "Recording")}

# display current defaults
echo "Current Default Playback Device: " $currPlay.name
echo "Current Default Playback Communication Device: " $currPlayComm.name 
echo "Current Default Recording Device: " $currRec.name 
echo "Current Default Recording Communication Device: " $currRecComm.name 
echo ""
Read-Host -Prompt "Press Enter to exit"