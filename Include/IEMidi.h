// Copyright © 2024 mozahzah (Incus Entertainment). All rights reserved.

#pragma once

#include "imgui.h"
#include "IEExtensions/ie.imgui.h"

#include "IECore/IECore.h"
#include "IECore/IERenderer.h"

#include "IEMidiEditor.h"
#include "IEMidiProcessor.h"
#include "IEMidiProfileManager.h"
#include "IEMidiTypes.h"

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
    IEMidiProcessor& GetMidiProcessor() const { return *m_MidiProcessor; }
    IEMidiProfileManager& GetMidiProfileManager() const { return *m_MidiProfileManager; }
    IEMidiEditor& GetMidiEditor() const { return *m_MidiEditor; }

public:
    IEAppState GetAppState() const;
    void SetAppState(IEAppState AppState);

    void OnPreFrameRender();
    void OnPostFrameRender();

    void DrawMidiDeviceSelectionWindow();
    IEResult OnMidiDeviceSelected(const std::string& DeviceName, uint32_t InputPortNumber);
    void DrawSelectedMidiDeviceMapperWindow();
    void SetupBackgroundProcess();

private:
    void ResetActiveMidiDeviceProfile();

private:
    static void OnAppWindowClosed(uint32_t WindowID, void* UserData);
    static void OnAppWindowRestored(uint32_t WindowID, void* UserData);
    static void OnRtMidiCallback(double TimeStamp, std::vector<unsigned char>* Message, void* UserData);
    static void OnRtMidiErrorCallback(RtMidiError::Type RtMidiErrorType, const std::string& ErrorText, void* UserData);

private:
    std::shared_ptr<IERenderer> m_Renderer;
    std::unique_ptr<RtMidiIn> m_MidiIn;
    std::unique_ptr<RtMidiOut> m_MidiOut;

private:
    std::shared_ptr<IEMidiProcessor> m_MidiProcessor;
    std::unique_ptr<IEMidiProfileManager> m_MidiProfileManager;
    std::unique_ptr<IEMidiEditor> m_MidiEditor;

private:
    IEAppState m_AppState = IEAppState::None;
    bool m_bOpenMidiEditorWindow = false;
};