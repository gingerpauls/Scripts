#include <string.h>
#include <stdio.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <functiondiscoverykeys_devpkey.h>

struct Device
{
    IMMDevice* Device;
    IPropertyStore* PropertyStore;
    IAudioEndpointVolume* AudioEndpointVolume;
    IMMEndpoint* Endpoint;
};

static UINT NumDevices;
static Device* AllDevices;

static LPWSTR defaultPlayback;
static LPWSTR defaultCommunicationPlayback;
static LPWSTR defaultRecording;
static LPWSTR defaultCommunicationRecording;

static void InitializeAllDevices(void)
{
    int MaxDevices = 256;
    AllDevices = (Device*)VirtualAlloc(0, sizeof(Device) * MaxDevices, MEM_COMMIT, PAGE_READWRITE);

    IMMDeviceEnumerator* deviceEnumerator = NULL;
    CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&deviceEnumerator));

    IMMDeviceCollection* deviceCollectionPtr = NULL;
    deviceEnumerator->EnumAudioEndpoints(eAll, DEVICE_STATE_ACTIVE, &deviceCollectionPtr);

    UINT count;
    deviceCollectionPtr->GetCount(&count);

    if (count > MaxDevices)
        return;

    IMMDevice* tempDevice;
    deviceEnumerator->GetDefaultAudioEndpoint(EDataFlow::eCapture, ERole::eMultimedia, &tempDevice);
    tempDevice->GetId(&defaultPlayback);

    deviceEnumerator->GetDefaultAudioEndpoint(EDataFlow::eCapture, ERole::eCommunications, &tempDevice);
    tempDevice->GetId(&defaultCommunicationPlayback);
    
    deviceEnumerator->GetDefaultAudioEndpoint(EDataFlow::eRender, ERole::eMultimedia, &tempDevice);
    tempDevice->GetId(&defaultRecording);

    deviceEnumerator->GetDefaultAudioEndpoint(EDataFlow::eRender, ERole::eCommunications, &tempDevice);
    tempDevice->GetId(&defaultCommunicationRecording);

    Device* currDevice = AllDevices;
    NumDevices = count;
    for (int i = 0; i < count; i++)
    {
        deviceCollectionPtr->Item(i, &currDevice->Device);
        currDevice->Device->OpenPropertyStore(STGM_READ, &currDevice->PropertyStore);
        currDevice->Device->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL, (void**)&currDevice->AudioEndpointVolume);
        currDevice->Device->QueryInterface(__uuidof(IMMEndpoint), (void**)&currDevice->Endpoint);
        currDevice++;
    }
}

int main(int numArguments, char* arguments[])
{
    CoInitialize(NULL);
    InitializeAllDevices();

    for (int i = 0; i < numArguments; i++)
    {
        
    }

    for (int i = 0; i < NumDevices; i++)
    {
        Device currDevice = AllDevices[i];

        PROPVARIANT varProperty;
        EDataFlow dataFlow;
        ERole role;
        float volumeScalar;
        float volumeLevel;
        BOOL isMute;
        LPWSTR id;

        currDevice.Endpoint->GetDataFlow(&dataFlow);
        currDevice.PropertyStore->GetValue(PKEY_Device_FriendlyName, &varProperty);
        currDevice.AudioEndpointVolume->GetMasterVolumeLevelScalar(&volumeScalar);
        currDevice.AudioEndpointVolume->GetMasterVolumeLevel(&volumeLevel);
        currDevice.AudioEndpointVolume->GetMute(&isMute);
        currDevice.Device->GetId(&id);

        char* deviceDefault;
        char* flow;
        if (dataFlow == EDataFlow::eCapture)
        {
            flow = "Playback";
            if (lstrcmpW(id, defaultCommunicationPlayback) == 0 && lstrcmpW(id, defaultPlayback) == 0)
                deviceDefault = "Playback Default and Default Communications";
            else if (lstrcmpW(id, defaultCommunicationPlayback) == 0)
                deviceDefault = "Playback Default Communications";
            else if (lstrcmpW(id, defaultPlayback) == 0)
                deviceDefault = "Playback Default";
            else 
				deviceDefault = "";
        }
        else if (dataFlow == EDataFlow::eRender)
        {
            flow = "Recording";
            if (lstrcmpW(id, defaultRecording) == 0 && lstrcmpW(id, defaultCommunicationRecording) == 0)
                deviceDefault = "Recording Default and Default Communications";
            else if (lstrcmpW(id, defaultCommunicationRecording) == 0)
                deviceDefault = "Recording Default Communications";
            else if (lstrcmpW(id, defaultRecording) == 0)
                deviceDefault = "Recording Default";
            else 
				deviceDefault = "";
        }
        else
        {
            flow = "Unknown";
            deviceDefault = "Unknown";
        }

        char* muted;
        if (isMute)
            muted = "true";
        else
            muted = "false";


        printf("Name: %ls\nVolume: %f\nLevel: %f\nDataFlow: %s\nIsMute: %s\nDetails: %s\n\n", varProperty.pwszVal, volumeScalar, volumeLevel, flow, muted, deviceDefault);
    }

	return 0;
}
