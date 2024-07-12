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

public:
    IEMidiProcessor& GetMidiProcessor() const { return *m_MidiProcessor; }
    IEMidiProfileManager& GetMidiProfileManager() const { return *m_MidiProfileManager; }
    IEMidiEditor& GetMidiEditor() const { return *m_MidiEditor; }

public:
    IEAppState GetAppState() const;
    void SetAppState(IEAppState AppState);

    void OnPreFrameRender();
    void OnPostFrameRender();

    void DrawMidiDeviceSelectionWindow();
    void DrawSelectedMidiDeviceEditorWindow();

private:
    static void OnAppWindowClosed(uint32_t WindowID, void* UserData);
    static void OnAppWindowRestored(uint32_t WindowID, void* UserData);

private:
    std::shared_ptr<IERenderer> m_Renderer;

private:
    std::shared_ptr<IEMidiProcessor> m_MidiProcessor;
    std::unique_ptr<IEMidiProfileManager> m_MidiProfileManager;
    std::unique_ptr<IEMidiEditor> m_MidiEditor;

private:
    IEAppState m_AppState = IEAppState::None;
};