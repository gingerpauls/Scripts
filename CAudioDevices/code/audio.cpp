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

struct Memory {
	UINT NumDevices;
	Device* Devices;

	IMMDeviceEnumerator* DeviceEnumerator;
	IPolicyConfig* PolicyConfig;
};

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

static void GetDefaultDevices(Memory* memory, DefaultDevices* defaultDevices)
{
	IMMDevice* device;
	memory->DeviceEnumerator->GetDefaultAudioEndpoint(EDataFlow::eRender, ERole::eMultimedia, &device);
	device->GetId(&defaultDevices->Playback);

	memory->DeviceEnumerator->GetDefaultAudioEndpoint(EDataFlow::eRender, ERole::eCommunications, &device);
	device->GetId(&defaultDevices->CommunicationPlayback);

	memory->DeviceEnumerator->GetDefaultAudioEndpoint(EDataFlow::eCapture, ERole::eMultimedia, &device);
	device->GetId(&defaultDevices->Recording);

	memory->DeviceEnumerator->GetDefaultAudioEndpoint(EDataFlow::eCapture, ERole::eCommunications, &device);
	device->GetId(&defaultDevices->CommunicationRecording);
}

static void PopulateAllDevices(Memory* memory)
{
	DefaultDevices defaultDevices;
	GetDefaultDevices(memory, &defaultDevices);

	for (int i = 0; i < memory->NumDevices; i++)
	{
		PopulateInfo(&memory->Devices[i], &defaultDevices);
	}
}

static void InitializeAndPopulateAllDevices(Memory** memory)
{
    int MaxDevices = 256;
	if (*memory == NULL)
		*memory = (Memory*)VirtualAlloc(0, sizeof(Memory), MEM_COMMIT, PAGE_READWRITE);

	if ((*memory)->Devices == NULL)
		(*memory)->Devices = (Device*)VirtualAlloc(0, sizeof(Device) * MaxDevices, MEM_COMMIT, PAGE_READWRITE);

	if ((*memory)->DeviceEnumerator == NULL)
		CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(*memory->DeviceEnumerator));

	if ((*memory)->PolicyConfig == NULL)
		CoCreateInstance(__uuidof(CPolicyConfigClient), NULL, CLSCTX_ALL, __uuidof(IPolicyConfig), (LPVOID*)*memory->PolicyConfig);

    IMMDeviceCollection* deviceCollectionPtr = NULL;
	(*memory)->DeviceEnumerator->EnumAudioEndpoints(eAll, DEVICE_STATE_ACTIVE, &deviceCollectionPtr);

    UINT count;
    deviceCollectionPtr->GetCount(&count);

	if (count > MaxDevices)
	{
		printf("Too many devices. Max is %i.\nStopping.\n", MaxDevices);
        return;
	}

    Device* currDevice = memory->Devices;
    memory->NumDevices = count;
    for (int i = 0; i < count; i++)
    {
        deviceCollectionPtr->Item(i, &currDevice->Device);
        currDevice->Device->OpenPropertyStore(STGM_READ, &currDevice->PropertyStore);
        currDevice->Device->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL, (void**)&currDevice->AudioEndpointVolume);
        currDevice->Device->QueryInterface(__uuidof(IMMEndpoint), (void**)&currDevice->Endpoint);

        currDevice++;
    }

	PopulateAllDevices(memory);
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

static void SetDevicesWhere(Memory* memory, float volumeScalar, BOOL mute, const wchar_t* pattern, bool invert)
{
	for (int i = 0; i < memory->NumDevices; i++)
	{
		Device* device = &memory->Devices[i];

		bool isMatch = match(pattern, device->Info.Name, 0, 0);
		if (invert)
			isMatch = !isMatch;

		if (!isMatch)
			continue;

		device->AudioEndpointVolume->SetMasterVolumeLevelScalar(volumeScalar, &GUID_NULL);
		device->AudioEndpointVolume->SetMute(mute, &GUID_NULL);
	}
}

static bool SetDefaultDevicesWhere(Memory* memory, ERole role, EDataFlow dataFlow, const wchar_t* pattern)
{
	bool flag = false;
	for (int i = 0; i < memory->NumDevices; i++)
	{
		Device* device = &memory->Devices[i];

		if (!match(pattern, device->Info.Name, 0, 0) || device->Info.DataFlow != dataFlow)
			continue;

		memory->PolicyConfig->SetDefaultEndpoint(device->Info.Id, role);
		flag = true;
		break;
	}

	return flag;
}

static void RandomizeAllDevices(Memory* memory)
{
	srand(time(NULL));

	for (int i = 0; i < memory->NumDevices; i++)
	{
		Device* device = &memory->Devices[i];

		float randomScalar = (float)rand() / (float)(RAND_MAX);
		float randomMute = (float)rand() / (float)(RAND_MAX);
		BOOL mute = randomMute >= 0.5;

		float randomDefault = (float)rand() / (float)(RAND_MAX);

		float randomDefaultCommunication = (float)rand() / (float)(RAND_MAX);

		device->AudioEndpointVolume->SetMasterVolumeLevelScalar(randomScalar, &GUID_NULL);
		device->AudioEndpointVolume->SetMute(mute, &GUID_NULL);

		if (randomDefault < 0.25)
			SetDefaultDevicesWhere(memory, ERole::eMultimedia, device->Info.DataFlow, device->Info.Name);
		if (randomDefaultCommunication < 0.25)
			SetDefaultDevicesWhere(memory, ERole::eCommunications, device->Info.DataFlow, device->Info.Name);
	}
}

static void SetAstroDevices(Memory* memory)
{
	wchar_t* astroGame = L"*Astro*Game*";
	wchar_t* astroVoice = L"*Astro*Voice*";

	if (SetDefaultDevicesWhere(memory, ERole::eMultimedia, EDataFlow::eRender, astroGame))
		printf("Set Astro Default Playback Device\n");
	else 
		printf("Unable to find Astro Playback Device. Did not set Default Playback Device.\n");

	if (SetDefaultDevicesWhere(memory, ERole::eCommunications, EDataFlow::eRender, astroVoice))
		printf("Set Astro Default Playback Communication Device\n");
	else
		printf("Unable to find Astro Playback Communication Device. Did not set Default Playback Communication Device.\n");

	if (SetDefaultDevicesWhere(memory, ERole::eMultimedia, EDataFlow::eCapture, astroVoice))
		printf("Set Astro Default Recording Device\n");
	else
		printf("Unable to find Astro Recording Device. Did not set Default Recording Device.\n");

	if (SetDefaultDevicesWhere(memory, ERole::eCommunications, EDataFlow::eCapture, astroVoice))
		printf("Set Astro Default Recording Communication Device\n");
	else
		printf("Unable to find Astro Recording Communication Device. Did not set Default Recording Communication Device.\n");

	SetDevicesWhere(memory, 1.0, FALSE, astroGame, false);
	SetDevicesWhere(memory, 1.0, FALSE, astroVoice, false);
}

static void SetTCHeliconDevices(Memory* memory)
{
	wchar_t* TCsystem = L"*System*TC-Helicon*";
	wchar_t* TCchat = L"*System*TC-Helicon*";
	wchar_t* TCmic = L"*System*TC-Helicon*";

	if (SetDefaultDevicesWhere(memory, ERole::eMultimedia, EDataFlow::eRender, TCsystem))
		printf("Set TC-Helicon Default Playback Device\n");
	else
		printf("Unable to find TC-Helicon Playback Device. Did not set Default Playback Device.\n");

	if (SetDefaultDevicesWhere(memory, ERole::eCommunications, EDataFlow::eRender, TCchat))
		printf("Set TC-Helicon Default Playback Communication Device\n");
	else
		printf("Unable to find TC-Helicon Playback Communication Device. Did not set Default Playback Communication Device.\n");

	if (SetDefaultDevicesWhere(memory, ERole::eMultimedia, EDataFlow::eCapture, TCmic))
		printf("Set TC-Helicon Default Recording Device\n");
	else
		printf("Unable to find TC-Helicon Recording Device. Did not set Default Recording Device.\n");

	if (SetDefaultDevicesWhere(memory, ERole::eCommunications, EDataFlow::eCapture, TCmic))
		printf("Set TC-Helicon Default Recording Communication Device\n");
	else
		printf("Unable to find TC-Helicon Recording Communication Device. Did not set Default Recording Communication Device.\n");

	SetDevicesWhere(memory, 1.0, FALSE, TCsystem, false);
	SetDevicesWhere(memory, 1.0, FALSE, TCchat, false);
	SetDevicesWhere(memory, 1.0, FALSE, TCmic, false);
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

static void PrintAllDevices(Memory* memory)
{
	printf("------------ Playback Devices ------------\n");
	for (int i = 0; i < memory->NumDevices; i++) {
		if (memory->Devices[i].Info.DataFlow != EDataFlow::eRender)
			continue;

		PrintInfo(&memory->Devices[i].Info);
	}

	printf("------------ Recording Devices ------------\n");
	for (int i = 0; i < memory->NumDevices; i++) {
		if (memory->Devices[i].Info.DataFlow != EDataFlow::eCapture)
			continue;

		PrintInfo(&memory->Devices[i].Info);
	}
}

int main(int numArguments, char* arguments[])
{
	Memory* memory = NULL;

    CoInitialize(NULL);
	InitializeAndPopulateAllDevices(&memory);

	wchar_t clause[100];
	bool invalid = false;

	if (numArguments == 2)
	{
		if (strcmp(arguments[1], "-l") == 0)
		{
			PrintAllDevices(memory);
		}
		else if (strcmp(arguments[1], "-r") == 0)
		{
			RandomizeAllDevices(memory);
		}
		else if (strcmp(arguments[1], "-Astro") == 0)
		{
			SetAstroDevices(memory);
		}
		else if (strcmp(arguments[1], "-TC") == 0)
		{
			SetTCHeliconDevices(memory);
		}
		else
		{
			invalid = true;
		}
	}
	else if (numArguments == 3)
	{
		if (strcmp(arguments[1], "-u") == 0)
		{
			// Unmute all matching devices
			swprintf(clause, 100, L"%hs", arguments[2]);
			SetDevicesWhere(memory, 1.0, FALSE, clause, false);
		}
		else if (strcmp(arguments[1], "-m") == 0)
		{
			// Mute all matching devices
			swprintf(clause, 100, L"%hs", arguments[2]);
			SetDevicesWhere(memory, 0.0, TRUE, clause, false);
		}
		else if (strcmp(arguments[1], "-un") == 0)
		{
			// Unmute all non-matching devices
			swprintf(clause, 100, L"%hs", arguments[2]);
			SetDevicesWhere(memory, 1.0, FALSE, clause, true);
		}
		else if (strcmp(arguments[1], "-mn") == 0)
		{
			// Mute all non-matching devices
			swprintf(clause, 100, L"%hs", arguments[2]);
			SetDevicesWhere(memory, 0.0, TRUE, clause, true);
		}
		else
			invalid = true;
	}
	else
	{
		invalid = true;
	}
	
	if (invalid)
	{
		printf("Unknown or missing arguments.\n\n");
		printf(" -l\t\tList all playback and recording devices.\n");
		printf("\n");
		printf(" -r\t\tRandomize mute, volume, default, and default communication devices.\n");
		printf("\n");
		printf(" -u <clause>\tUnmute and max volume all devices matching given clause.\n");
		printf(" -un <clause>\tUnmute and max volume all devices NOT matching given clause.\n");
		printf("\n");
		printf(" -m <clause>\tMute and 0 volume all devices matching given clause.\n");
		printf(" -mn <clause>\tMute and 0 volume all devices NOT matching given clause.\n");
		printf("\n");
		printf(" -Astro\t\tSet Default devices to expected Astro devices.\n");
		printf(" -TC\t\tSet Default devices to expected TC-Helicon devices.\n");
	}

	return 0;
}