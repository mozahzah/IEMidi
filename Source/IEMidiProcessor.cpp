// Copyright © 2024 mozahzah (Incus Entertainment). All rights reserved.

#include "IEMidiProcessor.h"

IEResult IEMidiProcessor::ProcessMidiInputMessage(const std::vector<unsigned char>& MidiMessage)
{
    IEResult Result(IEResult::Type::Fail, "Failed to process Midi");

    if (IEAssert(MidiMessage.size() >= 3))
    {
        if (m_ActiveMidiDeviceProfile.has_value())
        {
            for (const IEMidiDeviceProperty& ActiveMidiDeviceProperty : m_ActiveMidiDeviceProfile->Properties)
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

IEResult IEMidiProcessor::SendMidiOutputMessage(const std::vector<unsigned char>& MidiMessage)
{
    IEResult Result(IEResult::Type::Fail, "Successfully to send midi output message");
    if (IEAssert(MidiMessage.size() >= 3))
    {
        if (m_ActiveMidiDeviceProfile.has_value())
        {
            GetMidiOut().sendMessage(&MidiMessage);

            Result.Type = IEResult::Type::Success;
            Result.Message = std::string("Successfully sent midi output message");
        }
    }
    return Result;
}

IEMidiDeviceProfile& IEMidiProcessor::GetActiveMidiDeviceProfile()
{
    return m_ActiveMidiDeviceProfile.value();
}

bool IEMidiProcessor::IsMidiDeviceProfileActive(const IEMidiDeviceProfile& MidiDeviceProfile) const
{
    return m_ActiveMidiDeviceProfile.value() == MidiDeviceProfile;
}

void IEMidiProcessor::ActivateMidiDeviceProfile(const IEMidiDeviceProfile& MidiDeviceProfile)
{
    RtMidiIn& MidiIn = GetMidiIn();
    RtMidiOut& MidiOut = GetMidiOut();

    if (!IsMidiDeviceProfileActive(MidiDeviceProfile))
    {
        MidiIn.openPort(MidiDeviceProfile.GetInputPortNumber());
        MidiOut.openPort(MidiDeviceProfile.GetOutputPortNumber());

        for (const std::vector<unsigned char>& MidiMessage : MidiDeviceProfile.InitialOutputMidiMessages)
        {
            MidiOut.sendMessage(&MidiMessage);
        }

        MidiIn.setCallback(&IEMidiProcessor::OnRtMidiCallback, this);
    }
}

void IEMidiProcessor::DeactivateMidiDeviceProfile(const IEMidiDeviceProfile& MidiDeviceProfile)
{
    if (IsMidiDeviceProfileActive(MidiDeviceProfile))
    {
        m_ActiveMidiDeviceProfile.reset();

        RtMidiIn& MidiIn = GetMidiIn();
        if (MidiIn.isPortOpen())
        {
            MidiIn.closePort();
            MidiIn.cancelCallback();
        }
        
        RtMidiOut& MidiOut = GetMidiOut();
        if (MidiOut.isPortOpen())
        {
            MidiOut.closePort();
        }
    }
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

void IEMidiProcessor::OnRtMidiCallback(double TimeStamp, std::vector<unsigned char>* Message, void* UserData)
{
    if (Message && UserData)
    {
        const std::vector<unsigned char>& MidiMessage = *Message;
        if (IEMidiProcessor* const MidiProcessor = reinterpret_cast<IEMidiProcessor*>(UserData))
        {
            MidiProcessor->ProcessMidiInputMessage(MidiMessage);

            // switch (IEMidiApp->GetAppState())
            // {
            //     case IEAppState::MidiDeviceEditor:
            //     {
            //         // if (IEMidiApp->m_ActiveMidiDeviceProfile.has_value())
            //         // {
            //         //     IEMidiDeviceProfile& ActiveMidiDeviceProfile = IEMidiApp->m_ActiveMidiDeviceProfile.value();
            //         //     const size_t MidiMessageSize = MidiMessage.size();
            //         //     for (IEMidiDeviceProperty& MidiDeviceProperty : ActiveMidiDeviceProfile.Properties)
            //         //     {
            //         //         if (IEMidiApp->GetMidiProcessor().IsMidiDevicePropertyActive(MidiDeviceProperty))
            //         //         {
            //         //             if (IEAssert(MidiMessageSize == MidiDeviceProperty.MidiMessage.size()))
            //         //             {
            //         //                 MidiDeviceProperty.MidiMessage = MidiMessage;
            //         //                 IEMidiApp->GetMidiProcessor().DeactivateMidiDeviceProperty(MidiDeviceProperty);
            //         //             }
            //         //         }
            //         //     }
            //         // }
            //         break;
            //     }
            //     case IEAppState::Background:
            //     {
                    
            //         break;
            //     }
            //     default:
            //     {
            //         break;
            //     }
            // }
        }
    }
}

void IEMidiProcessor::OnRtMidiErrorCallback(RtMidiError::Type RtMidiErrorType, const std::string& ErrorText, void* UserData)
{
    IELOG_ERROR("%s", ErrorText.c_str());
}
