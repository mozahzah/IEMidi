// Copyright © 2024 mozahzah (Incus Entertainment). All rights reserved.

#pragma once

#include "ryml.hpp"
#include "ryml_std.hpp"

#include "IECore.h"

enum class IEMidiMessageType : uint8_t
{
    None,
    NoteOnOff,
    ControlChange,
};

struct IEMidiDeviceProperty
{
public:
    IEMidiMessageType MidiMessageType = IEMidiMessageType::None;
    std::vector<unsigned char> MidiMessage = std::vector<unsigned char>(3);

public:
    bool bIsRecording = false;
};

struct IEMidiDeviceProfile
{
public:
    IEMidiDeviceProfile() = delete;
    IEMidiDeviceProfile(const std::string& NameID, uint32_t InputPortNumber, uint32_t OutputPortNumer) :
                        Name(NameID), m_InputPortNumber(InputPortNumber), m_OutputPortNumber(OutputPortNumer) {}

    uint32_t GetInputPortNumber() const { return m_InputPortNumber; }
    uint32_t GetOutputPortNumber() const { return m_OutputPortNumber; }

public:
    std::string Name;
    std::vector<std::vector<unsigned char>> InitialOutputMidiMessages;
    std::vector<IEMidiDeviceProperty> Properties;

private:
    uint32_t m_InputPortNumber = -1;
    uint32_t m_OutputPortNumber = -1;
};

class IEMidiProfileManager
{
public:
    IEMidiProfileManager();
    
public:
    IEResult SaveProfile(const IEMidiDeviceProfile& MidiDeviceProfile) const;
    IEResult LoadProfile(IEMidiDeviceProfile& MidiDeviceProfile) const;
    IEResult RemoveProfile(const IEMidiDeviceProfile& MidiDeviceProfile) const;

private:
    std::string ExtractFileContent(const std::filesystem::path& FilePath) const;
};