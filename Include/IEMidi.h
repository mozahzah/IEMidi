// Copyright © 2024 mozahzah (Incus Entertainment). All rights reserved.

#pragma once

#include "imgui.h"
#include "RtMidi.h"

#include "IECore.h"
#include "IEMidiEditor.h"
#include "IEMidiProfileManager.h"
#include "IERenderer.h"

enum class IEAppState : uint16_t
{
    Loading,
    MidiDeviceSelection,
    MidiDeviceEditor,
    Background,
    None
};

class IEMidi
{
public:
    IEMidi();

public:
    IERenderer& GetRenderer() const { return *m_Renderer; }
    RtMidiIn& GetMidiIn() const { return *m_MidiIn; }
    RtMidiOut& GetMidiOut() const { return *m_MidiOut; }
    IEMidiProfileManager& GetMidiProfileManager() const { return *m_MidiProfileManager; }
    IEMidiEditor& GetMidiEditor() const { return *m_MidiEditor; }

    void SetAppState(IEAppState AppState);

    void OnPreFrameRender();
    void OnPostFrameRender();

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
    std::unique_ptr<IEMidiProfileManager> m_MidiProfileManager;
    std::unique_ptr<IEMidiEditor> m_MidiEditor;

    IEAppState m_AppState = IEAppState::None;

    std::optional<IEMidiDeviceProfile> m_SelectedMidiDeviceProfile;

    bool m_bOpenMidiEditorWindow = false;
};