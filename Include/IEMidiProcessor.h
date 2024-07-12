// Copyright © 2024 mozahzah (Incus Entertainment). All rights reserved.

#pragma once

#include "RtMidi.h"

#include "IEActions/IEAction.h"
#include "IECore/IECore.h"

#include "IEMidiTypes.h"

class IEMidiProcessor
{
public:
    IEMidiProcessor() :
        m_MidiIn(std::make_unique<RtMidiIn>()),
        m_MidiOut(std::make_unique<RtMidiOut>()),
        m_VolumeAction(IEAction::GetVolumeAction()),
        m_MuteAction(IEAction::GetMuteAction()),
        m_ConsoleCommandAction(IEAction::GetConsoleCommandAction()),
        m_OpenFileAction(IEAction::GetOpenFileAction())
    {
        m_MidiIn->setErrorCallback(&IEMidiProcessor::OnRtMidiErrorCallback);
        m_MidiOut->setErrorCallback(&IEMidiProcessor::OnRtMidiErrorCallback);
    };

public:
    RtMidiIn& GetMidiIn() const { return *m_MidiIn; }
    RtMidiOut& GetMidiOut() const { return *m_MidiOut; }
    
public:
    IEResult ProcessMidiInputMessage(const std::vector<unsigned char>& MidiMessage);
    IEResult SendMidiOutputMessage(const std::vector<unsigned char>& MidiMessage);

    std::vector<std::string> GetAvailableMidiDevices() const;
    void ActivateMidiDeviceProfile(const std::string& MidiDeviceName);
    void DeactivateMidiDeviceProfile();
    IEMidiDeviceProfile& GetActiveMidiDeviceProfile();

private:
    static void OnRtMidiCallback(double TimeStamp, std::vector<unsigned char>* Message, void* UserData);
    static void OnRtMidiErrorCallback(RtMidiError::Type RtMidiErrorType, const std::string& ErrorText, void* UserData);

private:
    std::unique_ptr<RtMidiIn> m_MidiIn;
    std::unique_ptr<RtMidiOut> m_MidiOut;

private:
    std::optional<IEMidiDeviceProfile> m_ActiveMidiDeviceProfile;

private:
    std::unique_ptr<IEAction_Volume> m_VolumeAction;
    std::unique_ptr<IEAction_Mute> m_MuteAction;
    std::unique_ptr<IEAction_ConsoleCommand> m_ConsoleCommandAction;
    std::unique_ptr<IEAction_OpenFile> m_OpenFileAction;
};