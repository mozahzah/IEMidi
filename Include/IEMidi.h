// Copyright © 2024 mozahzah (Incus Entertainment). All rights reserved.

#pragma once

#include "IERenderer.h"
#include "RtMidi.h"

enum class IEAppState : uint16_t
{
    Loading,
    MidiDeviceSelection,
    MidiDeviceMapper,
    Background,
    None
};

class IEMidi
{
private:
    struct IEMidiDevice
    {
        std::string DeviceName;
        uint32_t InputPortNumber = -1;
        uint32_t OutputPortNumber = -1;
    };

public:
    IEMidi();

public:
    IERenderer& GetRenderer() const { return *m_Renderer; }
    RtMidiIn& GetMidiIn() const { return *m_MidiIn; }
    RtMidiOut& GetMidiOut() const { return *m_MidiOut; }

    void SetAppState(IEAppState AppState);

    void PreFrameRender();
    void PostFrameRender();

    void DrawMidiDeviceSelectionWindow();
    IEResult OnMidiDeviceSelected(const std::string& DeviceName, uint32_t InputPortNumber);
    void DrawSelectedMidiDeviceMapperWindow();

private:
    static void OnRtMidiCallback(double TimeStamp, std::vector<unsigned char>* Message, void* UserData);
    static void OnRtMidiErrorCallback(RtMidiError::Type RtMidiErrorType, const std::string& ErrorText, void* UserData);

private:
    std::unique_ptr<IERenderer> m_Renderer;
    std::unique_ptr<RtMidiIn> m_MidiIn;
    std::unique_ptr<RtMidiOut> m_MidiOut;

    IEAppState m_AppState = IEAppState::None;
    std::optional<IEMidiDevice> m_CurrentDeviceSelected;
};