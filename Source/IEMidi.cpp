// Copyright © 2024 mozahzah (Incus Entertainment). All rights reserved.

#include "IEMidi.h"

IEMidi::IEMidi() :
#ifdef GLFW_INCLUDE_VULKAN
    m_Renderer(std::make_shared<IERenderer_Vulkan>()),
#endif
    m_MidiProcessor(std::make_shared<IEMidiProcessor>()),
    m_MidiProfileManager(std::make_unique<IEMidiProfileManager>()),
    m_MidiEditor(std::make_unique<IEMidiEditor>(m_MidiProcessor))
{
    m_Renderer->AddOnWindowCloseCallbackFunc(OnAppWindowClosed, this);
    m_Renderer->AddOnWindowRestoreCallbackFunc(OnAppWindowRestored, this);
}

IEAppState IEMidi::GetAppState() const
{
    return m_AppState;
}

void IEMidi::SetAppState(IEAppState AppState)
{
    m_AppState = AppState;
    GetRenderer().PostEmptyEvent();
}

void IEMidi::OnPreFrameRender()
{
    switch (m_AppState)
    {
        case IEAppState::MidiDeviceSelection:
        {
            DrawMidiDeviceSelectionWindow();
            break;
        }
        case IEAppState::MidiDeviceEditor:
        {
            DrawSelectedMidiDeviceEditorWindow();
            break;
        }
        default:
        {
            break;
        }
    }
}

void IEMidi::OnPostFrameRender()
{}

void IEMidi::DrawMidiDeviceSelectionWindow()
{
    static constexpr uint32_t WindowFlags = ImGuiWindowFlags_NoResize |
                                            ImGuiWindowFlags_NoMove |
                                            ImGuiWindowFlags_NoScrollbar |
                                            ImGuiWindowFlags_NoScrollWithMouse |
                                            ImGuiWindowFlags_NoCollapse |
                                            ImGuiWindowFlags_NoDocking;

    ImGuiViewport& MainViewport = *ImGui::GetMainViewport();
    const float WindowWidth = MainViewport.Size.x * 0.2f;
    const float WindowHeight = MainViewport.Size.y * 0.5f;
    const float WindowPosX = (MainViewport.Pos.x + (MainViewport.Size.x - WindowWidth) * 0.5f);
    const float WindowPosY = (MainViewport.Pos.y + (MainViewport.Size.y - WindowHeight) * 0.5f);

    ImGui::SetNextWindowSize(ImVec2(WindowWidth, WindowHeight), ImGuiCond_Always);
    ImGui::SetNextWindowPos(ImVec2(WindowPosX, WindowPosY));
    
    ImGui::Begin("Select Midi Device", nullptr, WindowFlags);

    IEMidiProcessor& MidiProcessor = GetMidiProcessor();
    for (const std::string MidiDeviceName : MidiProcessor.GetAvailableMidiDevices())
    {
        ImGui::SetCursorPosX((WindowWidth * 0.5f) - ImGui::CalcTextSize(MidiDeviceName.c_str()).x * 0.5f);
        ImGui::Text("%s", MidiDeviceName.c_str());

        static const float CursorPos = 50.0f;
        ImGui::SetCursorPosX(CursorPos);
        if (ImGui::Button("Activate"))
        {
            MidiProcessor.ActivateMidiDeviceProfile(MidiDeviceName);
            GetMidiProfileManager().LoadProfile(MidiProcessor.GetActiveMidiDeviceProfile());
            GetRenderer().CloseAppWindow();
            break;
        }

        ImGui::SameLine();

        static const char EditText[] = "Edit";
        ImGui::SetCursorPosX(WindowWidth - CursorPos - ImGui::CalcTextSize(EditText).x - ImGui::GetStyle().FramePadding.x * 2.0f);
        if (ImGui::Button(EditText))
        {
            MidiProcessor.ActivateMidiDeviceProfile(MidiDeviceName);
            GetMidiProfileManager().LoadProfile(MidiProcessor.GetActiveMidiDeviceProfile());
            SetAppState(IEAppState::MidiDeviceEditor);
            break;
        }

        ImGui::Separator();
    }

    ImGui::End();
}

void IEMidi::DrawSelectedMidiDeviceEditorWindow()
{
    static constexpr uint32_t WindowFlags = ImGuiWindowFlags_NoResize |
                                            ImGuiWindowFlags_NoMove |
                                            ImGuiWindowFlags_NoCollapse |
                                            ImGuiWindowFlags_NoTitleBar |
                                            ImGuiWindowFlags_NoDocking;

    ImGuiViewport& MainViewport = *ImGui::GetMainViewport();
    ImGui::SetNextWindowSize(ImVec2(MainViewport.Size.x, MainViewport.Size.y), ImGuiCond_Always);
    ImGui::SetNextWindowPos(ImVec2(MainViewport.Pos.x, MainViewport.Pos.y));

    IEMidiProcessor& MidiProcessor = GetMidiProcessor();
    IEMidiDeviceProfile& ActiveMidiDeviceProfile = MidiProcessor.GetActiveMidiDeviceProfile();

    const std::string WindowLabel = std::format("Editing {}", ActiveMidiDeviceProfile.Name);

    ImGui::Begin(WindowLabel.c_str(), nullptr, WindowFlags);

    m_MidiEditor->DrawMidiDeviceProfileEditor(ActiveMidiDeviceProfile);

    static const char SaveAndClose[] = "Save & Close";
    const ImVec2 CloseSelectableSize = ImVec2(ImGui::CalcTextSize(SaveAndClose));
    ImGui::SetCursorPos(ImVec2(MainViewport.Size.x - CloseSelectableSize.x - 20.0f, MainViewport.Size.y - CloseSelectableSize.y - 20.0f));
    if (ImGui::GreenButton(SaveAndClose))
    {
        if (GetMidiProfileManager().SaveProfile(ActiveMidiDeviceProfile))
        {
            MidiProcessor.DeactivateMidiDeviceProfile();
            SetAppState(IEAppState::MidiDeviceSelection);
        }
    }

    ImGui::End();
}

void IEMidi::OnAppWindowClosed(uint32_t WindowID, void* UserData)
{
    if (IEMidi* const IEMidiApp = reinterpret_cast<IEMidi*>(UserData))
    {
        IEMidiApp->SetAppState(IEAppState::Background);
    }
}

void IEMidi::OnAppWindowRestored(uint32_t WindowID, void* UserData)
{
    if (IEMidi* const IEMidiApp = reinterpret_cast<IEMidi*>(UserData))
    {
        IEMidiApp->GetMidiProcessor().DeactivateMidiDeviceProfile();
        IEMidiApp->SetAppState(IEAppState::MidiDeviceSelection);
    }
}