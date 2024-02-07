[System.Collections.ArrayList]$audioDevices = Get-AudioDevice -List


# save current defaults 
$currPlay = $audioDevices | where {($_.Default -eq "True") -and ($_.Type -eq "Playback")}
$currPlayComm = $audioDevices | where {($_.DefaultCommunication -eq "True") -and ($_.Type -eq "Playback")}
$currRec = $audioDevices | where {($_.Default -eq "True") -and ($_.Type -eq "Recording")}
$currRecComm = $audioDevices | where {($_.DefaultCommunication -eq "True") -and ($_.Type -eq "Recording")}

$audioDevices[0].name

# Specify the name of the audio device to enable "Listen to this device" for
$audioDeviceName = "Your Audio Device Name"

# Get the audio device object
#$audioDevice = Get-AudioDevice | Where-Object {$_.Name -eq $audioDeviceName}

# Enable "Listen to this device"
$currRec | Enable-AudioDeviceListen



