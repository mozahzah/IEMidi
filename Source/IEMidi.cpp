// Copyright © 2024 mozahzah (Incus Entertainment). All rights reserved.

#include "IEMidi.h"

IEMidi::IEMidi() : m_MidiIn(std::make_unique<RtMidiIn>()), m_MidiOut(std::make_unique<RtMidiOut>())
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

void IEMidi::PreFrameRender()
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
        case IEAppState::MidiDeviceMapper:
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

void IEMidi::PostFrameRender()
{
    
}

void IEMidi::DrawMidiDeviceSelectionWindow()
{
    ImGuiIO& IO = ImGui::GetIO();

    const uint32_t WindowFlags = ImGuiWindowFlags_NoResize |
                                ImGuiWindowFlags_NoMove |
                                ImGuiWindowFlags_NoScrollbar |
                                ImGuiWindowFlags_NoScrollWithMouse |
                                ImGuiWindowFlags_NoCollapse;

    const float WindowWidth = IO.DisplaySize.x * 0.2f;
    const float WindowHeight = IO.DisplaySize.y * 0.2f;

    const float WindowPosX = (IO.DisplaySize.x - WindowWidth) * 0.5f;
    const float WindowPosY = (IO.DisplaySize.y - WindowHeight) * 0.5f;

    ImGui::SetNextWindowSize(ImVec2(WindowWidth, WindowHeight), ImGuiCond_Always);
    ImGui::SetNextWindowPos(ImVec2(WindowPosX, WindowPosY));

    ImGui::Begin("Select Midi Device", nullptr, WindowFlags);
    
    RtMidiIn& MidiIn = GetMidiIn();
    for (int i = 0; i < MidiIn.getPortCount(); i++)
    {
        std::string DeviceName = MidiIn.getPortName(i);
        DeviceName.erase(DeviceName.size() - 2); // Remove port number and the space

        ImGui::SetCursorPosX((WindowWidth * (1.0f - 0.5f)) * 0.5f);
        if (ImGui::Button(DeviceName.c_str(), ImVec2(WindowWidth * 0.5f, ImGui::GetTextLineHeightWithSpacing())))
        {
            OnMidiDeviceSelected(DeviceName, i);
            break;
        }
    }

    ImGui::End();
}

IEResult IEMidi::OnMidiDeviceSelected(const std::string& DeviceName, uint32_t InputPortNumber)
{                      
    IEResult Result(IEResult::Type::Fail, "Failed to initialize Device");

    RtMidiIn& MidiIn = GetMidiIn();
    MidiIn.openPort(InputPortNumber);
    if (MidiIn.isPortOpen())
    {
        MidiIn.setCallback(&IEMidi::OnRtMidiCallback);

        RtMidiOut& MidiOut = GetMidiOut();
        for (int i = 0; i < MidiOut.getPortCount(); i++)
        {
            const uint32_t& OutputPortNumber = i;

            if (MidiOut.getPortName(OutputPortNumber).find(DeviceName) != std::string::npos)
            {
                MidiOut.openPort(OutputPortNumber);
                if (MidiOut.isPortOpen())
                {
                    const std::vector<unsigned char> MidiMessage = { 0x91, 0x00, 0x64 }; // Get from user settings file
                    MidiOut.sendMessage(&MidiMessage);

                    m_CurrentDeviceSelected = { DeviceName, InputPortNumber, OutputPortNumber };
                    m_AppState = IEAppState::MidiDeviceMapper;

                    Result.Type = IEResult::Type::Success;
                    std::ostringstream ResultMessageStream;
                    ResultMessageStream << "Successfully initialized " << DeviceName;
                    Result.Message = ResultMessageStream.str();
                    break;
                }
            }
        }        
    }
    
    return Result;
}

void IEMidi::DrawSelectedMidiDeviceMapperWindow()
{
    if (m_CurrentDeviceSelected.has_value())
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
        
        ImGui::Begin(m_CurrentDeviceSelected->DeviceName.c_str(), nullptr, WindowFlags);
        /* IEMidimapper */
        ImGui::End();
    }
}

void IEMidi::OnRtMidiCallback(double TimeStamp, std::vector<unsigned char>* Message, void* UserData)
{
    if (Message)
    {
        if (Message->size() == 3)
        {
            IELOG_INFO("%#04x, %#04x, %#04x", Message->at(0), Message->at(1), Message->at(2));
        }
    }
}

void IEMidi::OnRtMidiErrorCallback(RtMidiError::Type RtMidiErrorType, const std::string& ErrorText, void* UserData)
{
    IELOG_ERROR("%s", ErrorText.c_str());
}
