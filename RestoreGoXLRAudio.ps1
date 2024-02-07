
# [console]::WindowWidth=60; 
# [console]::WindowHeight=50; 
# [console]::BufferWidth=[console]::WindowWidth

echo "Restoring GoXLR audio devices..."

[System.Collections.ArrayList]$tcDevices = Get-AudioDevice -List | where name -like "*TC-Helicon*"

$maxVol = 100
$unmute = 0

for ( $index = 0; $index -lt $tcDevices.count; $index++ ) {
	Set-AudioDevice -ID $tcDevices[$index].id | Out-Null
	Set-AudioDevice -PlaybackVolume $maxVol
	Set-AudioDevice -PlaybackMute $unmute
	Set-AudioDevice -RecordingVolume $maxVol
	Set-AudioDevice -RecordingMute $unmute
}

$tcDevices | where Type -like "Playback" | where name -like "*System*TC-Helicon*"  | Set-AudioDevice -Verbose -DefaultOnly | Out-Null
$tcDevices | where Type -like "Playback" | where name -like "*Chat*TC-Helicon*"  | Set-AudioDevice -Verbose -CommunicationOnly | Out-Null
$tcDevices | where Type -like "Recording" | where name -like "*Mic*TC-Helicon*"  | Set-AudioDevice -Verbose | Out-Null