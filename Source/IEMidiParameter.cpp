// Copyright © 2024 mozahzah (Incus Entertainment). All rights reserved.

#include "IEMidiParameter.h"

namespace IEMidiParameter
{
    std::unique_ptr<IEParameter_Volume> GetVolumeParameter()
    {
#if defined (_WIN32)
        return std::make_unique<IEWinParameter_Volume>();
#elif defined (__APPLE__)
        return std::make_unique<IEAppleParameter_Volume>();
#else
        IEAssert(false);
#endif
    }
}

IEAppleParameter_Volume::IEAppleParameter_Volume()
{
    m_AudioDeviceID = GetAudioDeviceID();
}

float IEAppleParameter_Volume::GetVolume() const
{
    float Volume;
    UInt32 PropertySize = sizeof(float);
    const AudioObjectPropertyAddress PropertyAddress =
    {
        kAudioHardwareServiceDeviceProperty_VirtualMainVolume,
        kAudioDevicePropertyScopeOutput,
        kAudioObjectPropertyElementMain
    };
    
    AudioObjectGetPropertyData(m_AudioDeviceID, &PropertyAddress, 0, NULL, &PropertySize, &Volume);    
    return Volume;
}

void IEAppleParameter_Volume::SetVolume(float Volume)
{
    uint32_t PropertySize = sizeof(float);
    const AudioObjectPropertyAddress PropertyAddress =
    {
        kAudioHardwareServiceDeviceProperty_VirtualMainVolume,
        kAudioDevicePropertyScopeOutput,
        kAudioObjectPropertyElementMain
    };

    float FinalVolume = Volume / 127.0f;
    AudioObjectSetPropertyData(m_AudioDeviceID, &PropertyAddress, 0, NULL, PropertySize, &FinalVolume);
}

void IEAppleParameter_Volume::RegisterVolumeChangeCallback(const std::function<void(float)>& Callback)
{
    const AudioObjectPropertyAddress PropertyAddress =
    {
        kAudioHardwareServiceDeviceProperty_VirtualMainVolume,
        kAudioDevicePropertyScopeOutput,
        kAudioObjectPropertyElementMain
    };
    
    AudioObjectAddPropertyListener(m_AudioDeviceID, &PropertyAddress, &IEAppleParameter_Volume::VolumeChangeCallback, this);
    m_OnVolumeChangeCallback = Callback;
}

AudioDeviceID IEAppleParameter_Volume::GetAudioDeviceID()
{
    AudioDeviceID OutputDeviceID;
    uint32_t PropertySize = sizeof(AudioDeviceID);
    const AudioObjectPropertyAddress PropertyAddress =
    {
        kAudioHardwarePropertyDefaultOutputDevice,
        kAudioObjectPropertyScopeGlobal,
        kAudioObjectPropertyElementMain
    };
    
    AudioObjectGetPropertyData(kAudioObjectSystemObject, &PropertyAddress, 0, NULL, &PropertySize, &OutputDeviceID);
    return OutputDeviceID;
}

OSStatus IEAppleParameter_Volume::VolumeChangeCallback(AudioObjectID ObjectID, uint32_t NumberAddresses, const AudioObjectPropertyAddress* PropertyAddresses, void* UserData)
{
    OSStatus Status;
    if (IEAppleParameter_Volume* const AppleVolumeParameter = static_cast<IEAppleParameter_Volume*>(UserData))
    {
        if (AppleVolumeParameter->m_OnVolumeChangeCallback)
        {
            AppleVolumeParameter->m_OnVolumeChangeCallback(AppleVolumeParameter->GetVolume());
        }
        Status = noErr;
    }
    return Status;
}