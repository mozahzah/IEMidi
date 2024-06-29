// Copyright © 2024 mozahzah (Incus Entertainment). All rights reserved.

#include "IEMidi.h"

#include "IEUtils.h"

IEMidi::IEMidi() :  m_MidiIn(std::make_unique<RtMidiIn>()),
                    m_MidiOut(std::make_unique<RtMidiOut>()),
                    m_MidiProfileManager(std::make_unique<IEMidiProfileManager>()),
                    m_MidiEditor(std::make_unique<IEMidiEditor>())
{
    m_MidiIn->setErrorCallback(&IEMidi::OnRtMidiErrorCallback);
    m_MidiOut->setErrorCallback(&IEMidi::OnRtMidiErrorCallback);

#ifdef GLFW_INCLUDE_VULKAN
    m_Renderer = std::make_unique<IERenderer_Vulkan>();
#endif
}

void IEMidi::SetAppState(IEAppState AppState)
{
    m_AppState = AppState;
}

void IEMidi::OnPreFrameRender()
{
    switch (m_AppState)
    {
        case IEAppState::Loading:
        {
            break;
        }
        case IEAppState::MidiDeviceSelection:
        {
            DrawMidiDeviceSelectionWindow();
            break;
        }
        case IEAppState::MidiDeviceEditor:
        {
            DrawSelectedMidiDeviceMapperWindow();
            break;
        }
        case IEAppState::Background:
        {

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
    ImGuiIO& IO = ImGui::GetIO();

    const uint32_t WindowFlags = ImGuiWindowFlags_NoResize |
                                ImGuiWindowFlags_NoMove |
                                ImGuiWindowFlags_NoScrollbar |
                                ImGuiWindowFlags_NoScrollWithMouse |
                                ImGuiWindowFlags_NoCollapse;

    const float WindowWidth = IO.DisplaySize.x * 0.2f;
    const float WindowHeight = IO.DisplaySize.y * 0.5f;

    const float WindowPosX = (IO.DisplaySize.x - WindowWidth) * 0.5f;
    const float WindowPosY = (IO.DisplaySize.y - WindowHeight) * 0.5f;

    ImGui::SetNextWindowSize(ImVec2(WindowWidth, WindowHeight), ImGuiCond_Always);
    ImGui::SetNextWindowPos(ImVec2(WindowPosX, WindowPosY));

    ImGui::Begin("Select Midi Device", nullptr, WindowFlags);
    
    RtMidiIn& MidiIn = GetMidiIn();
    for (int InputPortNumber = 0; InputPortNumber < MidiIn.getPortCount(); InputPortNumber++)
    {
        const std::string MidiDeviceName = MidiIn.getPortName(InputPortNumber);

        ImGui::SetCursorPosX((WindowWidth * (1.0f - 0.5f)) * 0.5f);
        if (ImGui::Button(MidiDeviceName.c_str(), ImVec2(WindowWidth * 0.5f, ImGui::GetTextLineHeightWithSpacing())))
        {
            OnMidiDeviceSelected(MidiDeviceName, InputPortNumber);
            break;
        }
    }

    ImGui::End();
}

IEResult IEMidi::OnMidiDeviceSelected(const std::string& MidiDeviceName, uint32_t InputPortNumber)
{                      
    IEResult Result(IEResult::Type::Fail, "Failed to initialize midi device");

    RtMidiIn& MidiIn = GetMidiIn();
    if (!MidiIn.isPortOpen())
    {
        MidiIn.openPort(InputPortNumber);
    }

    RtMidiOut& MidiOut = GetMidiOut();
    if (!MidiOut.isPortOpen())
    {
        for (int OutputPortNumber = 0; OutputPortNumber < MidiOut.getPortCount(); OutputPortNumber++)
        {
            if (MidiOut.getPortName(OutputPortNumber).find(MidiDeviceName) != std::string::npos)
            {
                MidiOut.openPort(OutputPortNumber);
                m_SelectedMidiDeviceProfile = IEMidiDeviceProfile(MidiDeviceName, InputPortNumber, OutputPortNumber);
                break;
            }
        }
    }

    if (m_SelectedMidiDeviceProfile.has_value())
    {
        MidiIn.setCallback(&IEMidi::OnRtMidiCallback, &m_SelectedMidiDeviceProfile.value());

        if (GetMidiProfileManager().LoadProfile(m_SelectedMidiDeviceProfile.value()))
        {
            for (const std::vector<unsigned char>& MidiMessage : m_SelectedMidiDeviceProfile->InitialOutputMidiMessages)
            {
                MidiOut.sendMessage(&MidiMessage);
            }
        }

        m_AppState = IEAppState::MidiDeviceEditor;

        Result.Type = IEResult::Type::Success;
        Result.Message = std::format("Successfully initialized {} using input port {} and output port {}",
            m_SelectedMidiDeviceProfile->Name,
            m_SelectedMidiDeviceProfile->GetInputPortNumber(),
            m_SelectedMidiDeviceProfile->GetOutputPortNumber());
    }
    return Result;
}

void IEMidi::DrawSelectedMidiDeviceMapperWindow()
{
    if (m_SelectedMidiDeviceProfile.has_value())
    {
        ImGuiIO& IO = ImGui::GetIO();

        const uint32_t WindowFlags = ImGuiWindowFlags_NoResize |
                                ImGuiWindowFlags_NoMove |
                                ImGuiWindowFlags_NoScrollbar |
                                ImGuiWindowFlags_NoScrollWithMouse |
                                ImGuiWindowFlags_NoCollapse;


        const float WindowWidth = IO.DisplaySize.x * 0.5f;
        const float WindowHeight = IO.DisplaySize.y * 0.5f;

        const float WindowPosX = (IO.DisplaySize.x - WindowWidth) * 0.5f;
        const float WindowPosY = (IO.DisplaySize.y - WindowHeight) * 0.5f;

        ImGui::SetNextWindowSize(ImVec2(WindowWidth, WindowHeight), ImGuiCond_Always);
        ImGui::SetNextWindowPos(ImVec2(WindowPosX, WindowPosY));

        static bool bWindowOpen = true;
        const std::string WindowLabel = std::format("Editing {}", m_SelectedMidiDeviceProfile->Name);
        if (bWindowOpen)
        {
            ImGui::Begin(WindowLabel.c_str(), &bWindowOpen, ImGuiWindowFlags_NoSavedSettings);
            m_MidiEditor->DrawMidiDeviceProfileEditor(m_SelectedMidiDeviceProfile.value());
            ImGui::End();
        }
        else
        {
            GetMidiProfileManager().SaveProfile(m_SelectedMidiDeviceProfile.value());
            GetMidiIn().cancelCallback();
            SetAppState(IEAppState::MidiDeviceSelection);
        }
    }
}

void IEMidi::OnRtMidiCallback(double TimeStamp, std::vector<unsigned char>* Message, void* UserData)
{
    if (Message && UserData)
    {
        if (IEMidiDeviceProfile* const SelectedMidiDeviceProfile = reinterpret_cast<IEMidiDeviceProfile*>(UserData))
        {
            if (Message->size() == 3)
            {
                IELOG_INFO("%s: %#04x, %#04x, %#04x", SelectedMidiDeviceProfile->Name.c_str(), Message->at(0), Message->at(1), Message->at(2));
            }
        }
    }
}

void IEMidi::OnRtMidiErrorCallback(RtMidiError::Type RtMidiErrorType, const std::string& ErrorText, void* UserData)
{
    IELOG_ERROR("%s", ErrorText.c_str());
}
