#include <stdio.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <functiondiscoverykeys_devpkey.h>

struct DeviceData 
{

};

int main(int numArguments, char* arguments[])
{
    CoInitialize(NULL);

    for (int i = 0; i < numArguments; i++)
    {
        
    }

    IMMDeviceEnumerator* deviceEnumerator = NULL;
    if (!SUCCEEDED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&deviceEnumerator))))
    {
        printf("Failed to create MMDeviceEnumerator.");
        return 1;
    }

    // EDataFlow::eAll
    // EDataFlow::eCapture
    // EDataFlow::eRender
    IMMDeviceCollection* deviceCollectionPtr = NULL;
    if (!SUCCEEDED(deviceEnumerator->EnumAudioEndpoints(eAll, DEVICE_STATE_ACTIVE, &deviceCollectionPtr)))
    {
        printf("Failed to enumerate audio devices.");
        return 1;
    }

    UINT count;
    if (!SUCCEEDED(deviceCollectionPtr->GetCount(&count)))
    {
        printf("Failed to get count of devices in device collection.");
        return 1;
    }

    for (int i = 0; i < count; i++)
    {
        IMMDevice* device = NULL;
        deviceCollectionPtr->Item(i, &device);

        // Property Store
        IPropertyStore* propertyStore = NULL;
        device->OpenPropertyStore(STGM_READ, &propertyStore);
        
        PROPVARIANT varProperty;
        propertyStore->GetValue(PKEY_DeviceInterface_FriendlyName, &varProperty);

        // Audio Endpoint Volume
        IAudioEndpointVolume* volume = NULL;
        device->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL, (void**)&volume);

        float volumeScalar;
        float level;
        volume->GetMasterVolumeLevelScalar(&volumeScalar);
        volumeScalar = volumeScalar * 100;
        volume->GetMasterVolumeLevel(&level);
        //printf("Name: %ls\n\tVolume: %f\n\n", varProperty.pwszVal, volumeScalar);
        //volume->SetMasterVolumeLevelScalar(1.0,&GUID_NULL);
        //volume->GetMasterVolumeLevelScalar(&volumeScalar);

        printf("Name: %ls\n\tVolume: %f\n\tLevel: %f\n\n", varProperty.pwszVal, volumeScalar, level);
    }

	return 0;
}
