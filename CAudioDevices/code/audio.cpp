#include <string.h>
#include <stdio.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <functiondiscoverykeys_devpkey.h>

struct DeviceInfo {
	LPWSTR Id;
	LPWSTR Name;

	// Volume
	float VolumeScalar;
	float VolumeLevel;
	BOOL IsMute;

	// Device flags
	EDataFlow DataFlow;
	BOOL IsDefaultPlayback;
	BOOL IsDefaultCommunicationPlayback;
	BOOL IsDefaultRecording;
	BOOL IsDefaultCommunicationRecording;
};

struct Device
{
	DeviceInfo Info;

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

static void PopulateInfo(Device* device)
{
	device->Device->GetId(&device->Info.Id);

	PROPVARIANT varProperty;
	device->PropertyStore->GetValue(PKEY_Device_FriendlyName, &varProperty);
	device->Info.Name = varProperty.pwszVal;

	device->Endpoint->GetDataFlow(&device->Info.DataFlow);

	device->AudioEndpointVolume->GetMasterVolumeLevelScalar(&device->Info.VolumeScalar);
	device->AudioEndpointVolume->GetMasterVolumeLevel(&device->Info.VolumeLevel);
	device->AudioEndpointVolume->GetMute(&device->Info.IsMute);

	if (lstrcmpW(device->Info.Id, defaultPlayback) == 0)
		device->Info.IsDefaultPlayback = TRUE;
	if (lstrcmpW(device->Info.Id, defaultCommunicationPlayback) == 0)
		device->Info.IsDefaultCommunicationPlayback = TRUE;

	if (lstrcmpW(device->Info.Id, defaultRecording) == 0)
		device->Info.IsDefaultRecording = TRUE;
	if (lstrcmpW(device->Info.Id, defaultCommunicationRecording) == 0)
		device->Info.IsDefaultCommunicationRecording = TRUE;
}

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
    deviceEnumerator->GetDefaultAudioEndpoint(EDataFlow::eRender, ERole::eMultimedia, &tempDevice);
    tempDevice->GetId(&defaultPlayback);

    deviceEnumerator->GetDefaultAudioEndpoint(EDataFlow::eRender, ERole::eCommunications, &tempDevice);
    tempDevice->GetId(&defaultCommunicationPlayback);
    
    deviceEnumerator->GetDefaultAudioEndpoint(EDataFlow::eCapture, ERole::eMultimedia, &tempDevice);
    tempDevice->GetId(&defaultRecording);

    deviceEnumerator->GetDefaultAudioEndpoint(EDataFlow::eCapture, ERole::eCommunications, &tempDevice);
    tempDevice->GetId(&defaultCommunicationRecording);

    Device* currDevice = AllDevices;
    NumDevices = count;
    for (int i = 0; i < count; i++)
    {
        deviceCollectionPtr->Item(i, &currDevice->Device);
        currDevice->Device->OpenPropertyStore(STGM_READ, &currDevice->PropertyStore);
        currDevice->Device->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL, (void**)&currDevice->AudioEndpointVolume);
        currDevice->Device->QueryInterface(__uuidof(IMMEndpoint), (void**)&currDevice->Endpoint);

		PopulateInfo(currDevice);

        currDevice++;
    }
}

static char* BoolToString(BOOL _bool)
{
	if (_bool)
		return "True";
	return "False";
}

static void PrintInfo(DeviceInfo* info)
{
	printf("%ls\n", info->Name);
	printf("\tVolume: %f\n", info->VolumeScalar);
	printf("\tLevel: %f\n", info->VolumeLevel);
	printf("\tIsMute: %s\n", BoolToString(info->IsMute));

	char* flowString;
	if (info->DataFlow == EDataFlow::eCapture)
		flowString = "Recording";
	if (info->DataFlow == EDataFlow::eRender)
		flowString = "Playback";
	printf("\tType: %s\n", flowString);

	//TODO abstract this?
	if (info->DataFlow == EDataFlow::eCapture)
	{
		printf("\tIsDefault: %s\n", BoolToString(info->IsDefaultRecording));
		printf("\tIsDefaultCommunication: %s\n", BoolToString(info->IsDefaultCommunicationRecording));
	}

	if (info->DataFlow == EDataFlow::eRender)
	{
		printf("\tIsDefault: %s\n", BoolToString(info->IsDefaultPlayback));
		printf("\tIsDefaultCommunication: %s\n", BoolToString(info->IsDefaultCommunicationPlayback));
	}
	printf("\n");
}

static void PrintAllDevices()
{
	printf("------------ Playback Devices ------------\n");
	for (int i = 0; i < NumDevices; i++) {
		if (AllDevices[i].Info.DataFlow != EDataFlow::eRender)
			continue;

		PrintInfo(&AllDevices[i].Info);
	}

	printf("------------ Recording Devices ------------\n");
	for (int i = 0; i < NumDevices; i++) {
		if (AllDevices[i].Info.DataFlow != EDataFlow::eCapture)
			continue;

		PrintInfo(&AllDevices[i].Info);
	}
}

int main(int numArguments, char* arguments[])
{
    CoInitialize(NULL);
    InitializeAllDevices();
	PrintAllDevices();

	/*
	playback volume
	recording volume

	playback mute
	recording mute

	set volume
	set default
	set mute
	*/

	/*
	all devices unmute and max volume

	TC Helicon unmute and max volume

	randomize all mute and volume
	*/

	return 0;
}