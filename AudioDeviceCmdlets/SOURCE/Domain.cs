using CoreAudioApi;
using System;

namespace AudioDeviceCmdlets
{
    public static class Domain
    {
        public static void Print(MMDevice device)
        {
            var message = $"Name: {device.FriendlyName}\nVolume: {device.AudioEndpointVolume.MasterVolumeLevelScalar * 100f}\nMute: {device.AudioEndpointVolume.Mute}\nDataFlow: {device.DataFlow.ToReadableString()}\n";

            Console.WriteLine(message);
        }

        public static string ToReadableString(this EDataFlow source)
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
