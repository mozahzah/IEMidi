// Copyright © 2024 mozahzah (Incus Entertainment). All rights reserved.

#include "IEMidiProcessor.h"

#include "IEMidiParameter.h"

IEResult IEMidiProcessor::ProcessMidiMessage(const std::vector<unsigned char>& MidiMessage) const
{
    IEResult Result(IEResult::Type::Fail, "Failed to process Midi");

    for (const IEMidiDeviceProfile ActiveMidiDeviceProfile : m_ActiveMidiDeviceProfiles)
    {
        for (const IEMidiDeviceProperty& ActiveMidiDeviceProperty : ActiveMidiDeviceProfile.Properties)
        {
            if (ActiveMidiDeviceProperty.MidiMessage[0] == MidiMessage[0] &&
                ActiveMidiDeviceProperty.MidiMessage[1] == MidiMessage[1])
            {
                switch (ActiveMidiDeviceProperty.MidiParameterType)
                {
                case IEMidiParameterType::Volume:
                {
                    if (m_VolumeParameter)
                    {
                        m_VolumeParameter->SetVolume(static_cast<float>(MidiMessage[2]));
                    }
                    break;
                }
                case IEMidiParameterType::Mute:
                {
                    /* TODO */
                    break;
                }
                case IEMidiParameterType::ConsoleCommand:
                {
                    /* TODO */
                    break;
                }
                case IEMidiParameterType::OpenFile:
                {
                    /* TODO */
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
