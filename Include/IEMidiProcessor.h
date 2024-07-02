// Copyright © 2024 mozahzah (Incus Entertainment). All rights reserved.

#pragma once

#include "IECore/IECore.h"

#include "IEMidiParameter.h"
#include "IEMidiProfileManager.h"

class IEMidiProcessor
{
public:
    IEMidiProcessor() : m_VolumeParameter(IEMidiParameter::GetVolumeParameter()) {};
    
public:
    IEResult ProcessMidiMessage(const std::vector<unsigned char>& MidiMessage) const;

    bool IsMidiDeviceProfileActive(const IEMidiDeviceProfile& MidiDeviceProfile) const;
    void ActivateMidiDeviceProfile(const IEMidiDeviceProfile& MidiDeviceProfile);
    void DeactivateMidiDeviceProfile(const IEMidiDeviceProfile& MidiDeviceProfile);

private:
    std::unordered_set<IEMidiDeviceProfile, IEMidiDeviceProfileHash> m_ActiveMidiDeviceProfiles;
    std::unique_ptr<IEParameter_Volume> m_VolumeParameter;
};