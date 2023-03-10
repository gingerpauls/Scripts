#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <functiondiscoverykeys_devpkey.h>
#include "PolicyConfig.h"
#include <Windows.h>
#include <WinUser.h>


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
	if ( SUCCEEDED( DeviceEnumerator->GetDefaultAudioEndpoint(EDataFlow::eRender, ERole::eMultimedia, &device) ) ) {

		DeviceEnumerator->GetDefaultAudioEndpoint(EDataFlow::eRender, ERole::eMultimedia, &device);
		device->GetId(&defaultDevices->Playback);

		DeviceEnumerator->GetDefaultAudioEndpoint(EDataFlow::eRender, ERole::eCommunications, &device);
		device->GetId(&defaultDevices->CommunicationPlayback);
	}

	if ( SUCCEEDED( DeviceEnumerator->GetDefaultAudioEndpoint(EDataFlow::eCapture, ERole::eMultimedia, &device) ) ) {

		DeviceEnumerator->GetDefaultAudioEndpoint(EDataFlow::eCapture, ERole::eMultimedia, &device);
		device->GetId(&defaultDevices->Recording);

		DeviceEnumerator->GetDefaultAudioEndpoint(EDataFlow::eCapture, ERole::eCommunications, &device);
		device->GetId(&defaultDevices->CommunicationRecording);

	} 
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
	DeviceEnumerator->EnumAudioEndpoints(eAll, DEVICE_STATE_ACTIVE | DEVICE_STATE_DISABLED, &deviceCollectionPtr);
	
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

static void SetDevicesWhere(float volumeScalar, BOOL mute, const wchar_t* pattern, bool invert)
{
	for (int i = 0; i < NumDevices; i++)
	{
		Device* device = &AllDevices[i];

		//isMatch possibly doesn't do anything?
		bool isMatch = match(pattern, device->Info.Name, 0, 0);
		if (invert)
			isMatch = !isMatch;

		if (!isMatch)
			continue;

		device->AudioEndpointVolume->SetMasterVolumeLevelScalar(volumeScalar, &GUID_NULL);
		device->AudioEndpointVolume->SetMute(mute, &GUID_NULL);

		PolicyConfig->SetEndpointVisibility(device->Info.Id, true);
	}
}

static bool SetDefaultDevicesWhere(ERole role, EDataFlow dataFlow, const wchar_t* pattern)
{
	bool flag = false;
	for (int i = 0; i < NumDevices; i++)
	{
		Device* device = &AllDevices[i];

		if (!match(pattern, device->Info.Name, 0, 0) || device->Info.DataFlow != dataFlow)
			continue;

		PolicyConfig->SetDefaultEndpoint(device->Info.Id, role);
		flag = true;
		break;
	}

	return flag;
}

static void RandomizeAllDevices()
{
	srand(time(NULL));

	for (int i = 0; i < NumDevices; i++)
	{
		Device* device = &AllDevices[i];

		float randomScalar = (float)rand() / (float)(RAND_MAX);
		float randomMute = (float)rand() / (float)(RAND_MAX);
		float randomEnable = (float)rand() / (float)(RAND_MAX);

		BOOL mute = randomMute >= 0.5;
		BOOL enableDisable = randomEnable >= 0.5;

		float randomDefault = (float)rand() / (float)(RAND_MAX);
		float randomDefaultCommunication = (float)rand() / (float)(RAND_MAX);

		device->AudioEndpointVolume->SetMasterVolumeLevelScalar(randomScalar, &GUID_NULL);
		device->AudioEndpointVolume->SetMute(mute, &GUID_NULL);

		if (randomDefault < 0.25)
			SetDefaultDevicesWhere(ERole::eMultimedia, device->Info.DataFlow, device->Info.Name);
		if (randomDefaultCommunication < 0.25)
			SetDefaultDevicesWhere(ERole::eCommunications, device->Info.DataFlow, device->Info.Name);
		
		PolicyConfig->SetEndpointVisibility(device->Info.Id, enableDisable);
	}
}

static void SetAstroDevices()
{
	wchar_t* astroGame = L"*Astro*Game*";
	wchar_t* astroVoice = L"*Astro*Voice*";

	if (SetDefaultDevicesWhere(ERole::eMultimedia, EDataFlow::eRender, astroGame))
		printf("Set Astro Default Playback Device\n");
	else 
		printf("Unable to find Astro Playback Device. Did not set Default Playback Device.\n");

	if (SetDefaultDevicesWhere(ERole::eCommunications, EDataFlow::eRender, astroVoice))
		printf("Set Astro Default Playback Communication Device\n");
	else
		printf("Unable to find Astro Playback Communication Device. Did not set Default Playback Communication Device.\n");

	if (SetDefaultDevicesWhere(ERole::eMultimedia, EDataFlow::eCapture, astroVoice))
		printf("Set Astro Default Recording Device\n");
	else
		printf("Unable to find Astro Recording Device. Did not set Default Recording Device.\n");

	if (SetDefaultDevicesWhere(ERole::eCommunications, EDataFlow::eCapture, astroVoice))
		printf("Set Astro Default Recording Communication Device\n");
	else
		printf("Unable to find Astro Recording Communication Device. Did not set Default Recording Communication Device.\n");

	SetDevicesWhere(1.0, FALSE, astroGame, false);
	SetDevicesWhere(1.0, FALSE, astroVoice, false);
}

static void SetTCHeliconDevices()
{
	wchar_t* TCsystem = L"*System*TC-Helicon*";
	wchar_t* TCchat = L"*Chat*TC-Helicon*";
	wchar_t* TCmic = L"*Mic*TC-Helicon*";

	if (SetDefaultDevicesWhere(ERole::eMultimedia, EDataFlow::eRender, TCsystem))
		printf("Set TC-Helicon Default Playback Device\n");
	else
		printf("Unable to find TC-Helicon Playback Device. Did not set Default Playback Device.\n");

	if (SetDefaultDevicesWhere(ERole::eCommunications, EDataFlow::eRender, TCchat))
		printf("Set TC-Helicon Default Playback Communication Device\n");
	else
		printf("Unable to find TC-Helicon Playback Communication Device. Did not set Default Playback Communication Device.\n");

	if (SetDefaultDevicesWhere(ERole::eMultimedia, EDataFlow::eCapture, TCmic))
		printf("Set TC-Helicon Default Recording Device\n");
	else
		printf("Unable to find TC-Helicon Recording Device. Did not set Default Recording Device.\n");

	if (SetDefaultDevicesWhere(ERole::eCommunications, EDataFlow::eCapture, TCmic))
		printf("Set TC-Helicon Default Recording Communication Device\n");
	else
		printf("Unable to find TC-Helicon Recording Communication Device. Did not set Default Recording Communication Device.\n");

	SetDevicesWhere(1.0, FALSE, TCsystem, false);
	SetDevicesWhere(1.0, FALSE, TCchat, false);
	SetDevicesWhere(1.0, FALSE, TCmic, false);
}

static char* BoolToString(BOOL _bool)
{
	if (_bool)
		return "True";
	return "False";
}

static void PrintInfo(DeviceInfo* info)
{
	printf("%ls", info->Name);
	if (info->IsDefaultPlayback)
		printf(" * ");
	if (info->IsDefaultCommunicationPlayback)
		printf(" ** ");
	if (info->IsDefaultRecording)
		printf(" * ");
	if (info->IsDefaultCommunicationRecording)
		printf(" ** ");
	printf("\n");

	int simpleVolumeScalar = (info->VolumeScalar)*100;
	printf("\tVolume: %i\n", simpleVolumeScalar);

	char* muteState = "";
	if (info->IsMute == TRUE) 
	{
		muteState = "Muted";
	}
	else 
	{
		muteState = "Unmuted";
	}
	printf("\t%s\n", muteState);

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
	HWND console = GetConsoleWindow();
	RECT ConsoleRect;
	GetWindowRect(console, &ConsoleRect);
	MoveWindow(console, ConsoleRect.left, ConsoleRect.top, 500, 1400, TRUE);


    CoInitialize(NULL);
	InitializeAndPopulateAllDevices();

	wchar_t clause[100];
	bool invalid = false;

	if (numArguments == 2)
	{
		if (strcmp(arguments[1], "-l") == 0)
		{
			PrintAllDevices();
		}
		else if (strcmp(arguments[1], "-r") == 0)
		{
			RandomizeAllDevices();
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
			invalid = true;
		}
	}
	else if (numArguments == 3)
	{
		wchar_t clause[100];
		swprintf(clause, 100, L"%hs", arguments[2]);

		if (strcmp(arguments[1], "-u") == 0)
		{
			// Unmute all matching devices
			SetDevicesWhere(1.0, FALSE, clause, false);
		}
		else if (strcmp(arguments[1], "-m") == 0)
		{
			// Mute all matching devices
			SetDevicesWhere(0.0, TRUE, clause, false);
		}
		else if (strcmp(arguments[1], "-un") == 0)
		{
			// Unmute all non-matching devices
			SetDevicesWhere(1.0, FALSE, clause, true);
		}
		else if (strcmp(arguments[1], "-mn") == 0)
		{
			// Mute all non-matching devices
			SetDevicesWhere(0.0, TRUE, clause, true);
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