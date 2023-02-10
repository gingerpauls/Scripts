#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <functiondiscoverykeys_devpkey.h>
#include "PolicyConfig.h"

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

struct DefaultDevices {
	LPWSTR Playback;
	LPWSTR CommunicationPlayback;

	LPWSTR Recording;
	LPWSTR CommunicationRecording;
};

static UINT NumDevices;
static Device* AllDevices;

IMMDeviceEnumerator* DeviceEnumerator;
IPolicyConfig* PolicyConfig;

static void PopulateInfo(Device* device, DefaultDevices* defaultDevices)
{
	device->Device->GetId(&device->Info.Id);

	PROPVARIANT varProperty;
	device->PropertyStore->GetValue(PKEY_Device_FriendlyName, &varProperty);
	device->Info.Name = varProperty.pwszVal;

	device->Endpoint->GetDataFlow(&device->Info.DataFlow);

	device->AudioEndpointVolume->GetMasterVolumeLevelScalar(&device->Info.VolumeScalar);
	device->AudioEndpointVolume->GetMasterVolumeLevel(&device->Info.VolumeLevel);
	device->AudioEndpointVolume->GetMute(&device->Info.IsMute);

	if (lstrcmpW(device->Info.Id, defaultDevices->Playback) == 0)
		device->Info.IsDefaultPlayback = TRUE;
	if (lstrcmpW(device->Info.Id, defaultDevices->CommunicationPlayback) == 0)
		device->Info.IsDefaultCommunicationPlayback = TRUE;

	if (lstrcmpW(device->Info.Id, defaultDevices->Recording) == 0)
		device->Info.IsDefaultRecording = TRUE;
	if (lstrcmpW(device->Info.Id, defaultDevices->CommunicationRecording) == 0)
		device->Info.IsDefaultCommunicationRecording = TRUE;
}

static void GetDefaultDevices(DefaultDevices* defaultDevices)
{
	IMMDevice* device;
	DeviceEnumerator->GetDefaultAudioEndpoint(EDataFlow::eRender, ERole::eMultimedia, &device);
	device->GetId(&defaultDevices->Playback);

	DeviceEnumerator->GetDefaultAudioEndpoint(EDataFlow::eRender, ERole::eCommunications, &device);
	device->GetId(&defaultDevices->CommunicationPlayback);

	DeviceEnumerator->GetDefaultAudioEndpoint(EDataFlow::eCapture, ERole::eMultimedia, &device);
	device->GetId(&defaultDevices->Recording);

	DeviceEnumerator->GetDefaultAudioEndpoint(EDataFlow::eCapture, ERole::eCommunications, &device);
	device->GetId(&defaultDevices->CommunicationRecording);
}

static void PopulateAllDevices(void)
{
	DefaultDevices defaultDevices;
	GetDefaultDevices(&defaultDevices);

	for (int i = 0; i < NumDevices; i++)
	{
		PopulateInfo(&AllDevices[i], &defaultDevices);
	}
}

static void InitializeAndPopulateAllDevices(void)
{
    int MaxDevices = 256;
	NumDevices = 0;

	if (AllDevices == NULL)
		AllDevices = (Device*)VirtualAlloc(0, sizeof(Device) * MaxDevices, MEM_COMMIT, PAGE_READWRITE);

	if (DeviceEnumerator == NULL)
		CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&DeviceEnumerator));

	if (PolicyConfig == NULL)
		CoCreateInstance(__uuidof(CPolicyConfigClient), NULL, CLSCTX_ALL, __uuidof(IPolicyConfig), (LPVOID*)&PolicyConfig);

    IMMDeviceCollection* deviceCollectionPtr = NULL;
	DeviceEnumerator->EnumAudioEndpoints(eAll, DEVICE_STATE_ACTIVE, &deviceCollectionPtr);

    UINT count;
    deviceCollectionPtr->GetCount(&count);

	if (count > MaxDevices)
	{
		printf("Too many devices. Max is %i\n", MaxDevices);
        return;
	}

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

	PopulateAllDevices();
}

bool match(const wchar_t* pattern, const wchar_t* candidate, int p, int c) {
	if (pattern[p] == L'\0')
	{
		return candidate[c] == L'\0';
	}
	else if (pattern[p] == L'*')
	{
		for (; candidate[c] != L'\0'; c++)
		{
			if (match(pattern, candidate, p + 1, c))
				return true;
		}

		return match(pattern, candidate, p + 1, c);
	}
	else if (pattern[p] != candidate[c])
	{
		return false;
	}
	else
	{
		return match(pattern, candidate, p + 1, c + 1);
	}
}

static void SetDevicesWhere(float volumeScalar, BOOL mute, const wchar_t* pattern)
{
	for (int i = 0; i < NumDevices; i++)
	{
		Device* device = &AllDevices[i];

		if (!match(pattern, device->Info.Name, 0, 0))
			continue;

		device->AudioEndpointVolume->SetMasterVolumeLevelScalar(volumeScalar, &GUID_NULL);
		device->AudioEndpointVolume->SetMute(mute, &GUID_NULL);
	}
}

static void RandomizeAllDevices()
{
	srand(time(NULL));
	
	for (int i = 0; i < NumDevices; i++)
	{
		Device* device = &AllDevices[i];

		float randomScalar = (float)rand() / (float)(RAND_MAX);
		float randomMute = (float)rand() / (float)(RAND_MAX);
		BOOL mute = randomMute >= 0.5;

		device->AudioEndpointVolume->SetMasterVolumeLevelScalar(randomScalar, &GUID_NULL);
		device->AudioEndpointVolume->SetMute(mute, &GUID_NULL);
	}
}


static void SetDefaultDevicesWhere(ERole role, EDataFlow dataFlow, const wchar_t* pattern)
{
	for (int i = 0; i < NumDevices; i++)
	{
		Device* device = &AllDevices[i];

		if (!match(pattern, device->Info.Name, 0, 0) || device->Info.DataFlow != dataFlow)
			continue;

		PolicyConfig->SetDefaultEndpoint(device->Info.Id, role);
	}
}

static void SetAstroDevices()
{
	SetDefaultDevicesWhere(ERole::eMultimedia, EDataFlow::eRender, L"*Astro*Game*");
	SetDefaultDevicesWhere(ERole::eCommunications, EDataFlow::eRender, L"*Astro*Voice*");

	SetDefaultDevicesWhere(ERole::eMultimedia, EDataFlow::eCapture, L"*Astro*Voice*");
	SetDefaultDevicesWhere(ERole::eCommunications, EDataFlow::eCapture, L"*Astro*Voice*");
}

static void SetTCHeliconDevices()
{
	SetDefaultDevicesWhere(ERole::eMultimedia, EDataFlow::eRender, L"*System*TC-Helicon*");
	SetDefaultDevicesWhere(ERole::eCommunications, EDataFlow::eRender, L"*Chat*TC-Helicon*");

	SetDefaultDevicesWhere(ERole::eMultimedia, EDataFlow::eCapture, L"*Mic*TC-Helicon*");
	SetDefaultDevicesWhere(ERole::eCommunications, EDataFlow::eCapture, L"*Mic*TC-Helicon*");
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

		/*/
		PolicyConfig->SetDefaultEndpoint(AllDevices[i].Info.Id, ERole::eMultimedia);
		PolicyConfig->SetDefaultEndpoint(AllDevices[i].Info.Id, ERole::eCommunications);
		PopulateAllDevices();
		*/
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
	InitializeAndPopulateAllDevices();

	wchar_t clause[100];

	if (numArguments == 2)
	{
		if (strcmp(arguments[1], "-l") == 0)
		{
			PrintAllDevices();
		}
		else if (strcmp(arguments[1], "-Astro") == 0)
		{
			SetAstroDevices();
		}
		else if (strcmp(arguments[1], "-TC") == 0)
		{
			SetTCHeliconDevices();
		}
		else
		{
			printf("Unknown arguments...");
		}
	}
	else if (numArguments == 3)
	{
		if (strcmp(arguments[1], "-u") == 0)
		{
			// Unmute all matching devices
			swprintf(clause, 100, L"%hs", arguments[2]);
			SetDevicesWhere(1.0, FALSE, clause);
		}
		else if (strcmp(arguments[1], "-m") == 0)
		{
			// Mute all matching devices
			swprintf(clause, 100, L"%hs", arguments[2]);
			SetDevicesWhere(0.0, TRUE, clause);
		}
	}
	else
	{
		printf("Unknown arguments...");
	}

	return 0;
}