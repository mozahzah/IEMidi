// Copyright © 2024 mozahzah (Incus Entertainment). All rights reserved.

#pragma once

#include "IEActions/IEAction.h"
#include "IECore/IECore.h"

#include "IEMidiTypes.h"

class IEMidiProcessor
{
public:
    IEMidiProcessor() :
        m_VolumeAction(IEAction::GetVolumeAction()),
        m_MuteAction(IEAction::GetMuteAction()),
        m_ConsoleCommandAction(IEAction::GetConsoleCommandAction()),
        m_OpenFileAction(IEAction::GetOpenFileAction())
    {};

public:
    IEResult ProcessMidiMessage(const std::vector<unsigned char>& MidiMessage);

    bool IsMidiDeviceProfileActive(const IEMidiDeviceProfile& MidiDeviceProfile) const;
    void ActivateMidiDeviceProfile(const IEMidiDeviceProfile& MidiDeviceProfile);
    void DeactivateMidiDeviceProfile(const IEMidiDeviceProfile& MidiDeviceProfile);

    bool IsMidiDevicePropertyActive(const IEMidiDeviceProperty& MidiDeviceProperty) const;
    void ActivateMidiDeviceProperty(const IEMidiDeviceProperty& MidiDeviceProperty);
    void DeactivateMidiDeviceProperty(const IEMidiDeviceProperty& MidiDeviceProperty);
    
private:
    std::unordered_set<IEMidiDeviceProfile, IEMidiDeviceProfileHash> m_ActiveMidiDeviceProfiles;
    std::unordered_set<IEMidiDeviceProperty, IEMidiDevicePropertyHash> m_ActiveMidiDeviceProperties;

    std::unique_ptr<IEAction_Volume> m_VolumeAction;
    std::unique_ptr<IEAction_Mute> m_MuteAction;
    std::unique_ptr<IEAction_ConsoleCommand> m_ConsoleCommandAction;
    std::unique_ptr<IEAction_OpenFile> m_OpenFileAction;
};