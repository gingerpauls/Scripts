#install AudioDeviceCmdlets
#Install-Module -Name AudioDeviceCmdlets -Scope CurrentUser

#open Windows Sounds
control mmsys.cpl sounds

#Get-AudioDevice -List 
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

$tcHeliconPlay = Get-AudioDevice -List | where Type -like "Playback" | where name -like "*System*TC-Helicon*"  | Set-AudioDevice -Verbose -DefaultOnly
echo "TC-Helicon Default Playback Device: " $tcHeliconPlay.name
echo ""
$tcHeliconPlayComm = Get-AudioDevice -List | where Type -like "Playback" | where name -like "*Chat*TC-Helicon*"  | Set-AudioDevice -Verbose -DefaultOnly
echo "TC-Helicon Default Playback Communication Device: " $tcHeliconPlayComm.name
echo ""
$tcHeliconRec = Get-AudioDevice -List | where Type -like "Recording" | where name -like "*Mic*TC-Helicon*"  | Set-AudioDevice -Verbose -DefaultOnly
echo "TC-Helicon Default Recording Device: " $tcHeliconRec.name
echo ""
$tcHeliconRecComm = Get-AudioDevice -List | where Type -like "Recording" | where name -like "*Mic*TC-Helicon*"  | Set-AudioDevice -Verbose -DefaultOnly
echo "TC-Helicon Default Recording Communication Device: " $tcHeliconRecComm.name
echo ""

#checks
if ($currPlay.name -eq $tcHeliconPlay.name){
    echo "TC-Helicon Default Playback Device is set!"
} else {
    echo "TC-Helicon Default Playback Device is NOT set!"
}

if ($currPlayComm.name -eq $tcHeliconPlayComm.name){
    echo "TC-Helicon Default Playback Communication Device is set!"
} else {
    echo "TC-Helicon Default Playback Communication is NOT set!"
}

if ($currPlay.name -eq $tcHeliconPlay.name){
    echo "TC-Helicon Default Recording Device is set!"
} else {
    echo "TC-Helicon Default Recording Device is NOT set!"
}

if ($currRecComm.name -eq $tcHeliconRecComm.name){
    echo "TC-Helicon Default Recording Communication Device is set!"
} else {
    echo "TC-Helicon Default Recording Communication Device is NOT set!"
}

# Restore Broadcast Stream Mix, Sample, then restore Chat Mic
echo ""
echo "Fixing all TC-Helicon audio devices. Please wait..."
Get-AudioDevice -List | where Type -like "Recording" | where name -like "*Stream*TC-Helicon*"  | Set-AudioDevice -Verbose -DefaultOnly
Get-AudioDevice -List | where Type -like "Recording" | where name -like "*Sample*TC-Helicon*"  | Set-AudioDevice -Verbose -CommunicationOnly
Set-AudioDevice -RecordingMute 0
Set-AudioDevice -RecordingCommunicationMute 0
Set-AudioDevice -RecordingVolume 100
Set-AudioDevice -RecordingCommunicationVolume 100
Get-AudioDevice -List | where Type -like "Recording" | where name -like "*Chat Mic*TC-Helicon*"  | Set-AudioDevice -Verbose
Set-AudioDevice -RecordingMute 0
Set-AudioDevice -RecordingCommunicationMute 0
Set-AudioDevice -RecordingVolume 100
Set-AudioDevice -RecordingCommunicationVolume 100

# Restore Game, Music, Sample, then restore Chat and System
Get-AudioDevice -List | where Type -like "Playback" | where name -like "*Game*TC-Helicon*"   | Set-AudioDevice -Verbose -DefaultOnly
Get-AudioDevice -List | where Type -like "Playback" | where name -like "*Music*TC-Helicon*"  | Set-AudioDevice -Verbose -CommunicationOnly
Set-AudioDevice -PlaybackMute 0
Set-AudioDevice -PlaybackCommunicationMute 0
Set-AudioDevice -PlaybackVolume 100
Set-AudioDevice -PlaybackCommunicationVolume 100
Get-AudioDevice -List | where Type -like "Playback" | where name -like "*Sample*TC-Helicon*"  | Set-AudioDevice -Verbose -DefaultOnly
Set-AudioDevice -PlaybackMute 0
Set-AudioDevice -PlaybackVolume 100
Get-AudioDevice -List | where Type -like "Playback" | where name -like "*System*TC-Helicon*"  | Set-AudioDevice -Verbose -DefaultOnly
Get-AudioDevice -List | where Type -like "Playback" | where name -like "*Chat*TC-Helicon*"    | Set-AudioDevice -Verbose -CommunicationOnly
Set-AudioDevice -PlaybackMute 0
Set-AudioDevice -PlaybackCommunicationMute 0
Set-AudioDevice -PlaybackVolume 100
Set-AudioDevice -PlaybackCommunicationVolume 100

# final default settings
Get-AudioDevice -List | where Type -like "Playback" | where name -like "*System*TC-Helicon*"  | Set-AudioDevice -Verbose -DefaultOnly
Get-AudioDevice -List | where Type -like "Playback" | where name -like "*Chat*TC-Helicon*"  | Set-AudioDevice -Verbose -CommunicationOnly
Get-AudioDevice -List | where Type -like "Recording" | where name -like "*Mic*TC-Helicon*"  | Set-AudioDevice -Verbose

#Get-AudioDevice -List 
$currPlay = Get-AudioDevice -List | where {($_.Default -eq "True") -and ($_.Type -eq "Playback")}
echo "Current Default Playback Device: " $currPlay.name
echo ""
$currPlayComm = Get-AudioDevice -List | where {($_.DefaultCommunication -eq "True") -and ($_.Type -eq "Playback")}
echo "Current Default Playback Communication Device: " $currPlayComm.name 
echo ""
$currRec = Get-AudioDevice -List | where {($_.Default -eq "True") -and ($_.Type -eq "Recording")}
echo "Current Default Recording Device: " $currRec.name 
echo ""
$currRecComm = Get-AudioDevice -List | where {($_.DefaultCommunication -eq "True") -and ($_.Type -eq "Recording")}
echo "Current Default Recording Communication Device: " $currRecComm.name 
echo ""

#checks
if ($currPlay.name -eq $tcHeliconPlay.name){
    echo "TC-Helicon Default Playback Device is set!"
} else {
    echo "TC-Helicon Default Playback Device is NOT set!"
}

if ($currPlayComm.name -eq $tcHeliconPlayComm.name){
    echo "TC-Helicon Default Playback Communication Device is set!"
} else {
    echo "TC-Helicon Default Playback Communication is NOT set!"
}

if ($currPlay.name -eq $tcHeliconPlay.name){
    echo "TC-Helicon Default Recording Device is set!"
} else {
    echo "TC-Helicon Default Recording Device is NOT set!"
}

if ($currRecComm.name -eq $tcHeliconRecComm.name){
    echo "TC-Helicon Default Recording Communication Device is set!"
} else {
    echo "TC-Helicon Default Recording Communication Device is NOT set!"
}
