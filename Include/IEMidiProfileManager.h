// Copyright © 2024 mozahzah (Incus Entertainment). All rights reserved.

#pragma once

#include "IECore.h"

static constexpr size_t MIDI_MESSAGE_BYTE_COUNT = 3;

enum class IEMidiMessageType : uint8_t
{
    None,
    NoteOnOff,
    ControlChange,

    Count,
};

enum class IEMidiParameterType : uint8_t
{
    None,
    Volume,
    Mute,
    ConsoleCommand,
    OpenFile,

    Count,
};

struct IEMidiDeviceProperty
{
public:
    IEMidiMessageType MidiMessageType = IEMidiMessageType::None;
    IEMidiParameterType MidiParameterType = IEMidiParameterType::None;
    std::string ConsoleCommand = std::string();
    std::string OpenFilePath = std::string();
    std::vector<unsigned char> MidiMessage = std::vector<unsigned char>(MIDI_MESSAGE_BYTE_COUNT);

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