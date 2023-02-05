[console]::WindowWidth=30; 
[console]::WindowHeight=2; 
[console]::BufferWidth=[console]::WindowWidth
echo "Fixing GoXLR Audio..."
[System.Collections.ArrayList]$tcPlaybackDevices = Get-AudioDevice -List | where {($_.Type -eq "Playback")} | where name -like "*TC-Helicon*"
[System.Collections.ArrayList]$tcRecordingDevices = Get-AudioDevice -List | where {($_.Type -eq "Recording")} | where name -like "*TC-Helicon*"

$maxVol = 100
$unmute = 0

for ( $index = 0; $index -lt $tcPlaybackDevices.count; $index++ ) {

	Set-AudioDevice -ID $tcPlaybackDevices[$index].id | Out-Null
	Set-AudioDevice -PlaybackVolume $maxVol
	Set-AudioDevice -PlaybackMute $unmute
	
}

for ( $index = 0; $index -lt $tcRecordingDevices.count; $index++ ) {

	Set-AudioDevice -ID $tcRecordingDevices[$index].id  | Out-Null
	Set-AudioDevice -RecordingVolume $maxVol
	Set-AudioDevice -RecordingMute $unmute

}

Get-AudioDevice -List | where Type -like "Playback" | where name -like "*System*TC-Helicon*"  | Set-AudioDevice -Verbose -DefaultOnly | Out-Null
Get-AudioDevice -List | where Type -like "Playback" | where name -like "*Chat*TC-Helicon*"  | Set-AudioDevice -Verbose -CommunicationOnly | Out-Null
Get-AudioDevice -List | where Type -like "Recording" | where name -like "*Mic*TC-Helicon*"  | Set-AudioDevice -Verbose | Out-Null