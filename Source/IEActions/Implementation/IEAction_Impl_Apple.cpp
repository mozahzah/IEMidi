// Copyright © 2024 mozahzah (Incus Entertainment). All rights reserved.

#include "IEActions/Implementation/IEAction_Impl_Apple.h"

#if defined (__APPLE__)
IEAction_Volume_Impl_Apple::IEAction_Volume_Impl_Apple()
{
    m_AudioDeviceID = GetAudioDeviceID();
}

float IEAction_Volume_Impl_Apple::GetVolume() const
{
    float Volume = 0.0f;
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

void IEAction_Volume_Impl_Apple::SetVolume(float Volume)
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

void IEAction_Volume_Impl_Apple::RegisterVolumeChangeCallback(const std::function<void(float)>& Callback)
{
    const AudioObjectPropertyAddress PropertyAddress =
    {
        kAudioHardwareServiceDeviceProperty_VirtualMainVolume,
        kAudioDevicePropertyScopeOutput,
        kAudioObjectPropertyElementMain
    };
    
    AudioObjectAddPropertyListener(m_AudioDeviceID, &PropertyAddress, &IEAction_Volume_Impl_Apple::VolumeChangeCallback, this);
    m_OnVolumeChangeCallback = Callback;
}

AudioDeviceID IEAction_Volume_Impl_Apple::GetAudioDeviceID()
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

OSStatus IEAction_Volume_Impl_Apple::VolumeChangeCallback(AudioObjectID ObjectID, uint32_t NumberAddresses, const AudioObjectPropertyAddress* PropertyAddresses, void* UserData)
{
    OSStatus Status;
    if (IEAction_Volume_Impl_Apple* const AppleVolumeAction = static_cast<IEAction_Volume_Impl_Apple*>(UserData))
    {
        if (AppleVolumeAction->m_OnVolumeChangeCallback)
        {
            AppleVolumeAction->m_OnVolumeChangeCallback(AppleVolumeAction->GetVolume());
        }
        Status = noErr;
    }
    return Status;
}

IEAction_Mute_Impl_Apple::IEAction_Mute_Impl_Apple()
{
    m_AudioDeviceID = GetAudioDeviceID();
}

bool IEAction_Mute_Impl_Apple::GetMute() const
{
    uint32_t PropertySize = sizeof(uint32_t);
    uint32_t Mute = 0;
    const AudioObjectPropertyAddress PropertyAddress =
    {
        kAudioDevicePropertyMute,
        kAudioDevicePropertyScopeOutput,
        kAudioObjectPropertyElementMain
    };
    
    AudioObjectGetPropertyData(m_AudioDeviceID, &PropertyAddress, 0, NULL, &PropertySize, &Mute);    
    return Mute != 0;
}

void IEAction_Mute_Impl_Apple::SetMute(bool bMute)
{
    uint32_t PropertySize = sizeof(uint32_t);
    uint32_t Mute = bMute;
    const AudioObjectPropertyAddress PropertyAddress =
    {
        kAudioDevicePropertyMute,
        kAudioDevicePropertyScopeOutput,
        kAudioObjectPropertyElementMain
    };

    AudioObjectSetPropertyData(m_AudioDeviceID, &PropertyAddress, 0, NULL, PropertySize, &Mute);
}

void IEAction_Mute_Impl_Apple::RegisterMuteChangeCallback(const std::function<void(bool)>& Callback)
{
    const AudioObjectPropertyAddress PropertyAddress =
    {
        kAudioDevicePropertyMute,
        kAudioDevicePropertyScopeOutput,
        kAudioObjectPropertyElementMain
    };
    
    AudioObjectAddPropertyListener(m_AudioDeviceID, &PropertyAddress, &IEAction_Mute_Impl_Apple::MuteChangeCallback, this);
    m_OnMuteChangeCallback = Callback;
}

AudioDeviceID IEAction_Mute_Impl_Apple::GetAudioDeviceID()
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

OSStatus IEAction_Mute_Impl_Apple::MuteChangeCallback(AudioObjectID ObjectID, uint32_t NumberAddresses, const AudioObjectPropertyAddress* PropertyAddresses, void* UserData)
{
    OSStatus Status;
    if (IEAction_Mute_Impl_Apple* const AppleMuteAction = static_cast<IEAction_Mute_Impl_Apple*>(UserData))
    {
        if (AppleMuteAction->m_OnMuteChangeCallback)
        {
            AppleMuteAction->m_OnMuteChangeCallback(AppleMuteAction->GetMute());
        }
        Status = noErr;
    }
    return Status;
}
#endif