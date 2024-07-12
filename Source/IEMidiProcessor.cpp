// Copyright © 2024 mozahzah (Incus Entertainment). All rights reserved.

#include "IEMidiProcessor.h"

IEResult IEMidiProcessor::ProcessMidiInputMessage(const std::vector<unsigned char>& MidiMessage)
{
    IEResult Result(IEResult::Type::Fail, "Failed to process Midi");

    if (IEAssert(MidiMessage.size() >= 3))
    {
        if (m_ActiveMidiDeviceProfile.has_value())
        {
            for (IEMidiDeviceProperty& ActiveMidiDeviceProperty : m_ActiveMidiDeviceProfile->Properties)
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
                                                if (ActiveMidiDeviceProperty.bIsConsoleCommandActive)
                                                {
                                                    m_ConsoleCommandAction->ExecuteConsoleCommand(ActiveMidiDeviceProperty.ConsoleCommand, 0.0f);
                                                    ActiveMidiDeviceProperty.bIsConsoleCommandActive = false;
                                                }
                                                else
                                                {
                                                    m_ConsoleCommandAction->ExecuteConsoleCommand(ActiveMidiDeviceProperty.ConsoleCommand, 1.0f);
                                                    ActiveMidiDeviceProperty.bIsConsoleCommandActive = true;
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
    IEResult Result(IEResult::Type::Fail, "Failed to send midi output message");
    if (IEAssert(MidiMessage.size() >= 3))
    {
        if (m_ActiveMidiDeviceProfile)
        {
            GetMidiOut().sendMessage(&MidiMessage);

            Result.Type = IEResult::Type::Success;
            Result.Message = std::string("Successfully sent midi output message");
        }
    }
    return Result;
}

std::vector<std::string> IEMidiProcessor::GetAvailableMidiDevices() const
{
    std::vector<std::string> AvailableMidiDevices;

    RtMidiIn& MidiIn = GetMidiIn();
    for (int InputPortNumber = 0; InputPortNumber < MidiIn.getPortCount(); InputPortNumber++)
    {
        const std::string MidiDeviceName = MidiIn.getPortName(InputPortNumber);
        AvailableMidiDevices.emplace_back(MidiDeviceName.substr(0,
            MidiDeviceName.length() - (std::to_string(InputPortNumber).length() + 1)));
    }
    return AvailableMidiDevices;
}

IEMidiDeviceProfile& IEMidiProcessor::GetActiveMidiDeviceProfile()
{
    if (!m_ActiveMidiDeviceProfile.has_value())
    {
        IELOG_ERROR("No active midi device profile");
        abort();
    }

    return m_ActiveMidiDeviceProfile.value();
}

void IEMidiProcessor::ActivateMidiDeviceProfile(const std::string& MidiDeviceName)
{
    RtMidiIn& MidiIn = GetMidiIn();
    for (int InputPortNumber = 0; InputPortNumber < MidiIn.getPortCount(); InputPortNumber++)
    {
        const std::string MidiDeviceName = MidiIn.getPortName(InputPortNumber);

        RtMidiOut& MidiOut = GetMidiOut();
        for (int OutputPortNumber = 0; OutputPortNumber < MidiOut.getPortCount(); OutputPortNumber++)
        {
            if (MidiOut.getPortName(OutputPortNumber).find(MidiDeviceName) != std::string::npos ||
                MidiOut.getPortName(OutputPortNumber).find(MidiDeviceName.substr(0, MidiDeviceName.length() -
                (std::to_string(OutputPortNumber).length() + 1))) != std::string::npos)
            {
                m_ActiveMidiDeviceProfile = IEMidiDeviceProfile(MidiDeviceName, InputPortNumber, OutputPortNumber);

                MidiIn.openPort(m_ActiveMidiDeviceProfile->GetInputPortNumber());
                MidiOut.openPort(m_ActiveMidiDeviceProfile->GetOutputPortNumber());

                for (const std::vector<unsigned char>& MidiMessage : m_ActiveMidiDeviceProfile->InitialOutputMidiMessages)
                {
                    MidiOut.sendMessage(&MidiMessage);
                }

                MidiIn.setCallback(&IEMidiProcessor::OnRtMidiCallback, this);
                break;
            }
        }
    }     
}

void IEMidiProcessor::DeactivateMidiDeviceProfile()
{
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

    m_ActiveMidiDeviceProfile.reset();
}

void IEMidiProcessor::OnRtMidiCallback(double TimeStamp, std::vector<unsigned char>* Message, void* UserData)
{
    if (Message && UserData)
    {
        const std::vector<unsigned char>& MidiMessage = *Message;
        if (IEMidiProcessor* const MidiProcessor = reinterpret_cast<IEMidiProcessor*>(UserData))
        {
            if (MidiProcessor->m_ActiveMidiDeviceProfile)
            {
                for (IEMidiDeviceProperty& MidiDeviceProperty : MidiProcessor->m_ActiveMidiDeviceProfile->Properties)
                {
                    if (MidiDeviceProperty.bIsRecording)
                    {
                        MidiDeviceProperty.MidiMessage = MidiMessage;
                        MidiDeviceProperty.bIsRecording = false;
                    }
                }
            }
            
            MidiProcessor->ProcessMidiInputMessage(MidiMessage);
        }
    }
}

void IEMidiProcessor::OnRtMidiErrorCallback(RtMidiError::Type RtMidiErrorType, const std::string& ErrorText, void* UserData)
{
    IELOG_ERROR("%s", ErrorText.c_str());
}
