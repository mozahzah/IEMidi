// Copyright © 2024 mozahzah (Incus Entertainment). All rights reserved.

#pragma once

#if defined (_WIN32)
// TODO
#elif defined (__APPLE__)
#include <CoreAudio/CoreAudio.h>
#include <AudioToolbox/AudioToolbox.h>
#include <iostream>
#endif

#include "IECore/IECore.h"

class IEParameter_Volume
{
public:
    virtual ~IEParameter_Volume() = default;
    virtual float GetVolume() const { return 0.0f; }
    virtual void SetVolume(float Volume) {}
    virtual void RegisterVolumeChangeCallback(const std::function<void(float)>& Callback) {}
};

#if defined (__APPLE__)
class IEAppleParameter_Volume : public IEParameter_Volume
{
public:
    IEAppleParameter_Volume();

public:
    float GetVolume() const override;
    void SetVolume(float Volume) override;
    void RegisterVolumeChangeCallback(const std::function<void(float)>& Callback) override;

private:
    AudioDeviceID GetAudioDeviceID();
    static OSStatus VolumeChangeCallback(AudioObjectID ObjectID, uint32_t NumberAddresses, const AudioObjectPropertyAddress* PropertyAddresses, void* UserData);

private:
    AudioDeviceID m_AudioDeviceID;
    std::function<void(float)> m_OnVolumeChangeCallback;
};
#endif

namespace IEMidiParameter
{
    std::unique_ptr<IEParameter_Volume> GetVolumeParameter();
}