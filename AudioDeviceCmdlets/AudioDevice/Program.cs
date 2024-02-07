using AudioDeviceCmdlets;
using CoreAudioApi;
using System;
using System.Diagnostics;
using System.Text.RegularExpressions;

namespace AudioDevice
{
    class Program
    {
        static void Main(string[] args)
        {
            var stopwatch = new Stopwatch();
            stopwatch.Start();

            var deviceEnumerator = new MMDeviceEnumerator();
            var client = new PolicyConfigClient();
            var toolkit = new AudioDeviceCreationToolkit(deviceEnumerator);
            var allDevices = deviceEnumerator.EnumerateAudioEndPoints(EDataFlow.eAll, EDeviceState.DEVICE_STATE_ACTIVE);

            ApplyAllDevices(allDevices, x =>
            {
                var name = x.FriendlyName;

                x.AudioEndpointVolume.MasterVolumeLevelScalar = 1.0f;
                x.AudioEndpointVolume.Mute = false;

                /*
                if (name.Contains("Astro") && x.DataFlow == EDataFlow.eRender)
                {
                    if (name.Contains("Game"))
                        client.SetDefaultEndpoint(x.ID, ERole.eMultimedia);
                    else if (name.Contains("Voice"))
                        client.SetDefaultEndpoint(x.ID, ERole.eCommunications);
                }
                */

                /*
                if (Regex.IsMatch(x.FriendlyName, ".*Astro.*Voice") && x.DataFlow == EDataFlow.eRender)
                {
                    if (!toolkit.IsDefaultCommunication(x.ID))
                        client.SetDefaultEndpoint(x.ID, ERole.eCommunications);
                }
                */

                Print(x);
            });

            Console.WriteLine($"Elapsed MS: {stopwatch.ElapsedMilliseconds}");
        }

        static void ApplyAllDevices(MMDeviceCollection devices, Action<MMDevice> f)
        {
            for (int i = 0; i < devices.Count; i++)
            {
                f(devices[i]);
            }
        }

        static void Print(MMDevice device)
        {
            var message =
                $"Name: {device.FriendlyName}\n" +
                $"Volume: {device.AudioEndpointVolume.MasterVolumeLevelScalar * 100f}\n" +
                $"Mute: {device.AudioEndpointVolume.Mute}\n" +
                $"DataFlow: {ToReadableString(device.DataFlow)}\n";

            Console.WriteLine(message);
        }

        static string ToReadableString(EDataFlow source)
        {
            switch (source)
            {
                case EDataFlow.eAll:
                    return "Both";
                case EDataFlow.eCapture:
                    return "Recording";
                case EDataFlow.eRender:
                    return "Playback";
                case EDataFlow.EDataFlow_enum_count:
                    return "Count";

                default:
                    return "Unknown";
            }
        }
    }
}
