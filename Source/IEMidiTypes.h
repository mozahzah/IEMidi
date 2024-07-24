// SPDX-License-Identifier: GPL-2.0-only
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

enum class IEMidiActionType : uint8_t
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
private:
    static uint32_t MidiDevicePropertyIDGenerator;

public:
    IEMidiDeviceProperty() = delete;
    IEMidiDeviceProperty(const std::string& MidiDeviceNameID) :
        MidiDeviceName(MidiDeviceNameID),
        RuntimeID(MidiDevicePropertyIDGenerator++)
    {}

    IEMidiDeviceProperty& operator=(const IEMidiDeviceProperty& Other)
    {
        if (this != &Other)
        {
            MidiDeviceName = Other.MidiDeviceName;
            MidiMessageType = Other.MidiMessageType;
            MidiActionType = Other.MidiActionType;
            ConsoleCommand = Other.ConsoleCommand;
            OpenFilePath = Other.OpenFilePath;
            MidiMessage = Other.MidiMessage;
            bToggle = Other.bToggle;
        }
        return *this;
    }

    bool operator==(const IEMidiDeviceProperty& Other) const
    {
        return RuntimeID == Other.RuntimeID;
    }

public:
    const uint32_t RuntimeID;
    bool bIsRecording = false;
    bool bIsConsoleCommandActive = false;

public:
    std::string MidiDeviceName = std::string();
    IEMidiMessageType MidiMessageType = IEMidiMessageType::None;
    IEMidiActionType MidiActionType = IEMidiActionType::None;
    std::string ConsoleCommand = std::string();
    std::string OpenFilePath = std::string();
    std::vector<unsigned char> MidiMessage = std::vector<unsigned char>(MIDI_MESSAGE_BYTE_COUNT);
    bool bToggle = false;
};

struct IEMidiDevicePropertyHash
{
    size_t operator()(const IEMidiDeviceProperty& MidiDeviceProperty) const
    {
        return std::hash<uint32_t>()(MidiDeviceProperty.RuntimeID);
    }
};

struct IEMidiDeviceProfile
{
public:
    IEMidiDeviceProfile() = default;
    IEMidiDeviceProfile(const std::string& NameID, uint32_t InputPortNumber, uint32_t OutputPortNumer) :
                        Name(NameID), m_InputPortNumber(InputPortNumber), m_OutputPortNumber(OutputPortNumer) {}
    bool operator==(const IEMidiDeviceProfile& Other) const
    {
        return Name == Other.Name;
    }

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

struct IEMidiDeviceProfileHash
{
    size_t operator()(const IEMidiDeviceProfile& MidiDeviceProfile) const
    {
        return std::hash<std::string>()(MidiDeviceProfile.Name);
    }
};