
# [console]::WindowWidth=60; 
# [console]::WindowHeight=50; 
# [console]::BufferWidth=[console]::WindowWidth
echo "Getting audio device data... `n "

#Sets random mute, and volume to all audio devices (playback and recording)
[System.Collections.ArrayList]$audioDevices = Get-AudioDevice -List


# save current defaults 
$currPlay = $audioDevices | where {($_.Default -eq "True") -and ($_.Type -eq "Playback")}
$currPlayComm = $audioDevices | where {($_.DefaultCommunication -eq "True") -and ($_.Type -eq "Playback")}
$currRec = $audioDevices | where {($_.Default -eq "True") -and ($_.Type -eq "Recording")}
$currRecComm = $audioDevices | where {($_.DefaultCommunication -eq "True") -and ($_.Type -eq "Recording")}


#get audioDevices
echo "-------------Current Playback Devices-------------"

for ( $index = 0; $index -lt $audioDevices.count; $index++ ) {
	Set-AudioDevice -ID $audioDevices[$index].id | Out-Null

	if($audioDevices[$index].type -eq "Playback"){
		Write-Host -NoNewline $audioDevices[$index].name
		if($audioDevices[$index].name -eq $currPlay.name){
			Write-Host -NoNewline " *"
		}
		if($audioDevices[$index].name -eq $currPlayComm.name){
			Write-Host -NoNewline "	** "
		}
		Write-Host -NoNewline "		Vol: "
		Get-AudioDevice -PlaybackVolume | Write-Host -NoNewline
		Write-Host -NoNewline " / Mute: "
		Get-AudioDevice -PlaybackMute #| Write-Host -NoNewline
	}

}

#get recordingDevices
echo "`n-------------Current Recording Devices-------------"

for ( $index = 0; $index -lt $audioDevices.count; $index++ ) {
	Set-AudioDevice -ID $audioDevices[$index].id | Out-Null
	
	if($audioDevices[$index].type -eq "Recording"){
		Write-Host -NoNewline $audioDevices[$index].name
		if($audioDevices[$index].name -eq $currRec.name){
			Write-Host -NoNewline " *"
		}
		if($audioDevices[$index].name -eq $currRecComm.name){
			Write-Host -NoNewline "	** "
		}
		Write-Host -NoNewline "		Vol: "
		Get-AudioDevice -RecordingVolume | Write-Host -NoNewline
		Write-Host -NoNewline " / Mute: "
		Get-AudioDevice -RecordingMute #| Write-Host -NoNewline
	}

	
}

# restore previous audio devices
Set-AudioDevice -ID $currPlay.id -DefaultOnly | Out-Null
Set-AudioDevice -ID $currPlayComm.id -CommunicationOnly | Out-Null
Set-AudioDevice -ID $currRec.id -DefaultOnly | Out-Null
Set-AudioDevice -ID $currRecComm.id -CommunicationOnly| Out-Null

# Read-Host -Prompt "Press Enter to exit"