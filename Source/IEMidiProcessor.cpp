// Copyright © 2024 mozahzah (Incus Entertainment). All rights reserved.

#include "IEMidiProcessor.h"

IEResult IEMidiProcessor::ProcessMidiMessage(const std::vector<unsigned char>& MidiMessage)
{
    IEResult Result(IEResult::Type::Fail, "Failed to process Midi");

    if (IEAssert(MidiMessage.size() >= 3))
    {
        for (const IEMidiDeviceProfile& ActiveMidiDeviceProfile : m_ActiveMidiDeviceProfiles)
        {
            for (const IEMidiDeviceProperty& ActiveMidiDeviceProperty : ActiveMidiDeviceProfile.Properties)
            {
                if (ActiveMidiDeviceProperty.MidiMessage.size() >= 3 &&
                    ActiveMidiDeviceProperty.MidiMessage[0] == MidiMessage[0] &&
                    ActiveMidiDeviceProperty.MidiMessage[1] == MidiMessage[1])
                {
                    switch (ActiveMidiDeviceProperty.MidiActionType)
                    {
                        case IEMidiActionType::Volume:
                        {
                            if (m_VolumeAction)
                            {
                                Result.Type = IEResult::Type::Success;

                                const float Value = static_cast<float>(MidiMessage[2]);
                                m_VolumeAction->SetVolume(Value);
                            }
                            break;
                        }
                        case IEMidiActionType::Mute:
                        {
                            if (m_MuteAction)
                            {
                                Result.Type = IEResult::Type::Success;

                                if (ActiveMidiDeviceProperty.MidiMessageType == IEMidiMessageType::NoteOnOff)
                                {
                                    if (ActiveMidiDeviceProperty.bToggle)
                                    {
                                        const bool bOn = static_cast<unsigned int>(MidiMessage[2]) != 0;
                                        if (bOn)
                                        {
                                            if (m_MuteAction->GetMute())
                                            {
                                                m_MuteAction->SetMute(false);
                                            }
                                            else
                                            {
                                                m_MuteAction->SetMute(true);
                                            }
                                        }
                                    }
                                    else
                                    {
                                        const bool bMute = static_cast<bool>(MidiMessage[2]);
                                        m_MuteAction->SetMute(bMute);
                                    }
                                }
                            }
                            break;
                        }
                        case IEMidiActionType::ConsoleCommand:
                        {
                            if (m_ConsoleCommandAction)
                            {
                                Result.Type = IEResult::Type::Success;

                                switch (ActiveMidiDeviceProperty.MidiMessageType)
                                {
                                    case IEMidiMessageType::NoteOnOff:
                                    {
                                        if (ActiveMidiDeviceProperty.bToggle)
                                        {
                                            const bool bOn = static_cast<unsigned int>(MidiMessage[2]) != 0;
                                            if (bOn)
                                            {
                                                if (IsMidiDevicePropertyActive(ActiveMidiDeviceProperty))
                                                {
                                                    m_ConsoleCommandAction->ExecuteConsoleCommand(ActiveMidiDeviceProperty.ConsoleCommand, 0.0f);
                                                    DeactivateMidiDeviceProperty(ActiveMidiDeviceProperty);
                                                }
                                                else
                                                {
                                                    m_ConsoleCommandAction->ExecuteConsoleCommand(ActiveMidiDeviceProperty.ConsoleCommand, 1.0f);
                                                    ActivateMidiDeviceProperty(ActiveMidiDeviceProperty);
                                                }
                                            }
                                        }
                                        else
                                        {
                                            m_ConsoleCommandAction->ExecuteConsoleCommand(ActiveMidiDeviceProperty.ConsoleCommand, 1.0f);
                                        }
                                        break;
                                    }
                                    case IEMidiMessageType::ControlChange:
                                    {
                                        const float Value = static_cast<float>(MidiMessage[2]);
                                        m_ConsoleCommandAction->ExecuteConsoleCommand(ActiveMidiDeviceProperty.ConsoleCommand, Value);
                                        break;
                                    }
                                    default:
                                    {
                                        break;
                                    }
                                }
                            }
                            break;
                        }
                        case IEMidiActionType::OpenFile:
                        {
                            if (m_OpenFileAction)
                            {
                                Result.Type = IEResult::Type::Success;

                                if (ActiveMidiDeviceProperty.MidiMessageType == IEMidiMessageType::NoteOnOff)
                                {
                                    const bool bOn = static_cast<unsigned int>(MidiMessage[2]) != 0;
                                    if (bOn)
                                    {
                                        m_OpenFileAction->OpenFile(ActiveMidiDeviceProperty.OpenFilePath);
                                    }
                                }
                            }
                            break;
                        }
                        default:
                        {
                            break;
                        }
                    }
                }
            }
        }
    }
    if (Result.Type == IEResult::Type::Success)
    {
        Result.Message = std::string("Successfully processed Midi");
    }
    return Result;
}

bool IEMidiProcessor::IsMidiDeviceProfileActive(const IEMidiDeviceProfile& MidiDeviceProfile) const
{
    return m_ActiveMidiDeviceProfiles.contains(MidiDeviceProfile);
}

void IEMidiProcessor::ActivateMidiDeviceProfile(const IEMidiDeviceProfile& MidiDeviceProfile)
{
    m_ActiveMidiDeviceProfiles.emplace(MidiDeviceProfile);
}

void IEMidiProcessor::DeactivateMidiDeviceProfile(const IEMidiDeviceProfile& MidiDeviceProfile)
{
   m_ActiveMidiDeviceProfiles.erase(MidiDeviceProfile);
}

bool IEMidiProcessor::IsMidiDevicePropertyActive(const IEMidiDeviceProperty& MidiDeviceProperty) const
{
    return m_ActiveMidiDeviceProperties.contains(MidiDeviceProperty);
}

void IEMidiProcessor::ActivateMidiDeviceProperty(const IEMidiDeviceProperty& MidiDeviceProperty)
{
    m_ActiveMidiDeviceProperties.emplace(MidiDeviceProperty);
}

void IEMidiProcessor::DeactivateMidiDeviceProperty(const IEMidiDeviceProperty& MidiDeviceProperty)
{
    m_ActiveMidiDeviceProperties.erase(MidiDeviceProperty);
}