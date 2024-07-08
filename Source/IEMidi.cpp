// Copyright © 2024 mozahzah (Incus Entertainment). All rights reserved.

#include "IEMidi.h"

IEMidi::IEMidi() :
#ifdef GLFW_INCLUDE_VULKAN
    m_Renderer(std::make_shared<IERenderer_Vulkan>()),
#endif
    m_MidiIn(std::make_unique<RtMidiIn>()),
    m_MidiOut(std::make_unique<RtMidiOut>()),
    m_MidiProcessor(std::make_shared<IEMidiProcessor>()),
    m_MidiProfileManager(std::make_unique<IEMidiProfileManager>()),
    m_MidiEditor(std::make_unique<IEMidiEditor>(m_MidiProcessor))
{
    m_Renderer->AddOnWindowCloseCallbackFunc(OnAppWindowClosed, this);
    m_Renderer->AddOnWindowRestoreCallbackFunc(OnAppWindowRestored, this);

    m_MidiIn->setErrorCallback(&IEMidi::OnRtMidiErrorCallback);
    m_MidiOut->setErrorCallback(&IEMidi::OnRtMidiErrorCallback);
}

IEAppState IEMidi::GetAppState() const
{
    return m_AppState;
}

void IEMidi::SetAppState(IEAppState AppState)
{
    m_AppState = AppState;
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
            DrawSelectedMidiDeviceMapperWindow();
            GetRenderer().PostEmptyEvent();
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
    ImGuiIO& IO = ImGui::GetIO();

    static constexpr uint32_t WindowFlags = ImGuiWindowFlags_NoResize |
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
            if (MidiOut.getPortName(OutputPortNumber).find(MidiDeviceName) != std::string::npos ||
                MidiOut.getPortName(OutputPortNumber).find(MidiDeviceName.substr(0, MidiDeviceName.length() -
                (std::to_string(OutputPortNumber).length() + 1))) != std::string::npos)
            {
                MidiOut.openPort(OutputPortNumber);
                GetMidiProcessor().ActivateMidiDeviceProfile(IEMidiDeviceProfile(MidiDeviceName, InputPortNumber, OutputPortNumber));
                break;
            }
        }
    }

    if (GetMidiProfileManager().LoadProfile(GetMidiProcessor().GetActiveMidiDeviceProfile()))
    {
        SetAppState(IEAppState::MidiDeviceEditor);
        m_bOpenMidiEditorWindow = true;

        Result.Type = IEResult::Type::Success;
        Result.Message = std::format("Successfully initialized {} using input port {} and output port {}",
            m_ActiveMidiDeviceProfile->Name,
            m_ActiveMidiDeviceProfile->GetInputPortNumber(),
            m_ActiveMidiDeviceProfile->GetOutputPortNumber());
    }

    return Result;
}

void IEMidi::DrawSelectedMidiDeviceMapperWindow()
{
    IEMidiDeviceProfile& ActiveMidiDeviceProfile = GetMidiProcessor().GetActiveMidiDeviceProfile();

    ImGuiIO& IO = ImGui::GetIO();

    static constexpr uint32_t WindowFlags = ImGuiWindowFlags_NoResize |
                                            ImGuiWindowFlags_NoMove |
                                            ImGuiWindowFlags_NoCollapse |
                                            ImGuiWindowFlags_NoTitleBar;


    ImGui::SetNextWindowSize(ImVec2(IO.DisplaySize.x, IO.DisplaySize.y), ImGuiCond_Always);
    ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));

    const std::string WindowLabel = std::format("Editing {}", ActiveMidiDeviceProfile.Name);
    if (m_bOpenMidiEditorWindow)
    {
        ImGui::Begin(WindowLabel.c_str(), &m_bOpenMidiEditorWindow, WindowFlags);

        m_MidiEditor->DrawMidiDeviceProfileEditor(ActiveMidiDeviceProfile);

        const ImVec2 CloseSelectableSize = ImVec2(ImGui::CalcTextSize("Save & Close"));
        ImGui::SetCursorPos(ImVec2(IO.DisplaySize.x - CloseSelectableSize.x - 20.0f, IO.DisplaySize.y - CloseSelectableSize.y - 20.0f));
        if (ImGui::GreenButton("Save & Close"))
        {
            m_bOpenMidiEditorWindow = false;
        }
        ImGui::End();
    }
    else
    {
        if (GetMidiProfileManager().SaveProfile(ActiveMidiDeviceProfile))
        {
            SetAppState(IEAppState::MidiDeviceSelection);
            ResetActiveMidiDeviceProfile();
        }
    }
    
}

void IEMidi::SetupBackgroundProcess()
{
    RtMidiOut& MidiOut = GetMidiOut();
    RtMidiIn& MidiIn = GetMidiIn();

    IEMidiProcessor& MidiProcessor = GetMidiProcessor();
    IEMidiProfileManager& MidiProfileManager = GetMidiProfileManager();

    for (int InputPortNumber = 0; InputPortNumber < MidiIn.getPortCount(); InputPortNumber++)
    {
        const std::string MidiDeviceName = MidiIn.getPortName(InputPortNumber);
        for (int OutputPortNumber = 0; OutputPortNumber < MidiOut.getPortCount(); OutputPortNumber++)
        {
            if (MidiOut.getPortName(OutputPortNumber).find(MidiDeviceName) != std::string::npos ||
                MidiOut.getPortName(OutputPortNumber).find(MidiDeviceName.substr(0, MidiDeviceName.length() -
                (std::to_string(OutputPortNumber).length() + 1))) != std::string::npos)
            {
                IEMidiDeviceProfile MidiDeviceProfile = IEMidiDeviceProfile(MidiDeviceName, InputPortNumber, OutputPortNumber);
                if (!MidiProcessor.IsMidiDeviceProfileActive(MidiDeviceProfile))
                {
                    if (MidiProfileManager.HasProfile(MidiDeviceProfile))
                    {
                        if (MidiProfileManager.LoadProfile(MidiDeviceProfile))
                        {
                            MidiProcessor.ActivateMidiDeviceProfile(MidiDeviceProfile);
                            
                            MidiIn.openPort(InputPortNumber);
                            MidiOut.openPort(OutputPortNumber);

                            for (const std::vector<unsigned char>& MidiMessage : MidiDeviceProfile.InitialOutputMidiMessages)
                            {
                                MidiOut.sendMessage(&MidiMessage);
                            }

                            MidiIn.setCallback(&IEMidi::OnRtMidiCallback, this);
                            m_ActiveMidiDeviceProfile = MidiDeviceProfile;
                            break;
                        }
                    }
                }
            }
        }
    }
}

void IEMidi::ResetActiveMidiDeviceProfile()
{
    IEMidiProcessor& MidiProcessor = GetMidiProcessor();
    if (m_ActiveMidiDeviceProfile.has_value() &&
        MidiProcessor.IsMidiDeviceProfileActive(m_ActiveMidiDeviceProfile.value()))
    {
        MidiProcessor.DeactivateMidiDeviceProfile(m_ActiveMidiDeviceProfile.value());
    }
    m_ActiveMidiDeviceProfile.reset();

    RtMidiIn& MidiIn = GetMidiIn();
    if (MidiIn.isPortOpen())
    {
        MidiIn.closePort();
        MidiIn.cancelCallback();
    }
    
    RtMidiOut& MidiOut = GetMidiOut();
    if (MidiOut.isPortOpen())
    {
        MidiOut.closePort();
    }
}

void IEMidi::OnAppWindowClosed(uint32_t WindowID, void* UserData)
{
    if (IEMidi* const IEMidiApp = reinterpret_cast<IEMidi*>(UserData))
    {
        IEMidiApp->SetAppState(IEAppState::Background);
        IEMidiApp->ResetActiveMidiDeviceProfile();
        IEMidiApp->SetupBackgroundProcess();
    }
}

void IEMidi::OnAppWindowRestored(uint32_t WindowID, void* UserData)
{
    if (IEMidi* const IEMidiApp = reinterpret_cast<IEMidi*>(UserData))
    {
        IEMidiApp->SetAppState(IEAppState::MidiDeviceSelection);
        IEMidiApp->ResetActiveMidiDeviceProfile();
    }
}

void IEMidi::OnRtMidiCallback(double TimeStamp, std::vector<unsigned char>* Message, void* UserData)
{
    if (Message && UserData)
    {
        const std::vector<unsigned char>& MidiMessage = *Message;
        if (IEMidi* const IEMidiApp = reinterpret_cast<IEMidi*>(UserData))
        {
            switch (IEMidiApp->GetAppState())
            {
                case IEAppState::MidiDeviceEditor:
                {
                    if (IEMidiApp->m_ActiveMidiDeviceProfile.has_value())
                    {
                        IEMidiDeviceProfile& ActiveMidiDeviceProfile = IEMidiApp->m_ActiveMidiDeviceProfile.value();
                        const size_t MidiMessageSize = MidiMessage.size();
                        for (IEMidiDeviceProperty& MidiDeviceProperty : ActiveMidiDeviceProfile.Properties)
                        {
                            if (IEMidiApp->GetMidiProcessor().IsMidiDevicePropertyActive(MidiDeviceProperty))
                            {
                                if (IEAssert(MidiMessageSize == MidiDeviceProperty.MidiMessage.size()))
                                {
                                    MidiDeviceProperty.MidiMessage = MidiMessage;
                                    IEMidiApp->GetMidiProcessor().DeactivateMidiDeviceProperty(MidiDeviceProperty);
                                }
                            }
                        }
                    }
                    break;
                }
                case IEAppState::Background:
                {
                    IEMidiApp->GetMidiProcessor().ProcessMidiInputMessage(MidiMessage);
                    break;
                }
                default:
                {
                    break;
                }
            }
        }
    }
}

void IEMidi::OnRtMidiErrorCallback(RtMidiError::Type RtMidiErrorType, const std::string& ErrorText, void* UserData)
{
    IELOG_ERROR("%s", ErrorText.c_str());
}