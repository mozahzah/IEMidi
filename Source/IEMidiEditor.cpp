// Copyright © 2024 mozahzah (Incus Entertainment). All rights reserved.

#include "IEMidiEditor.h"

#include "imgui.h"
#include "IEExtensions/imgui_IE.h"

#include "IEUtils.h"

static constexpr size_t MidiDevicePropertyEditorColumnCount = 7;
static constexpr size_t MidiDeviceInitialOutputMessageEditorColumnCount = 2;
static constexpr float InputBoxSizeWidth = 150.0f;

void IEMidiEditor::DrawMidiDeviceProfileEditor(IEMidiDeviceProfile& MidiDeviceProfile) const
{
    static const ImVec2 PlusButtonSize = ImVec2(ImGui::GetFontSize() * 2.0f, ImGui::GetFontSize() * 2.0f);
    
    if (ImGui::BeginTable("Profile Midi Input Editor", 1))
    {
        for (std::vector<IEMidiDeviceProperty>::iterator It = MidiDeviceProfile.Properties.begin();
            It != MidiDeviceProfile.Properties.end();)
        {
            ImGui::PushID(It.base());
            ImGui::TableNextColumn();
            bool bDeleteRequested = false;
            DrawMidiDeviceInputPropertyEditor(*It, bDeleteRequested);
            It = bDeleteRequested ? MidiDeviceProfile.Properties.erase(It) : It+1;
            ImGui::PopID();
        }
        MidiDeviceProfile.Properties.shrink_to_fit();

        ImGui::TableNextColumn();
        if (ImGui::Button("+", PlusButtonSize))
        {
            MidiDeviceProfile.Properties.push_back(IEMidiDeviceProperty());
        }

        ImGui::EndTable();
    }
    
    if (ImGui::BeginTable("Profile Output Midi Message Editor", 1))
    {
        for (std::vector<std::vector<unsigned char>>::iterator It = MidiDeviceProfile.InitialOutputMidiMessages.begin();
            It != MidiDeviceProfile.InitialOutputMidiMessages.end();)
        {
            ImGui::PushID(It.base());
            ImGui::TableNextColumn();
            bool bDeleteRequested = false;
            DrawInitialOutputMessageEditor(*It, bDeleteRequested);
            It = bDeleteRequested ? MidiDeviceProfile.InitialOutputMidiMessages.erase(It) : It+1;
            ImGui::PopID();
        }
        MidiDeviceProfile.InitialOutputMidiMessages.shrink_to_fit();

        ImGui::TableNextColumn();
        if (ImGui::Button("+", PlusButtonSize))
        {
            MidiDeviceProfile.InitialOutputMidiMessages.push_back(std::vector<unsigned char>(MIDI_MESSAGE_BYTE_COUNT));
        }

        ImGui::EndTable();
    }
}

void IEMidiEditor::DrawMidiDeviceInputPropertyEditor(IEMidiDeviceProperty& MidiDeviceProperty, bool& bDeleteRequested) const
{
    if (ImGui::BeginTable("Midi Property Editor", MidiDevicePropertyEditorColumnCount, ImGuiTableFlags_Hideable | ImGuiTableFlags_SizingFixedFit))
    {
        ImGui::TableHeadersRow();

        static const char MessageTypesStringArray[static_cast<int>(IEMidiMessageType::Count)][std::size("-Select Message Type")] =
        {   "-Select Message Type",
            "NoteOnOff",
            "ControlChange" };
        ImGui::TableNextColumn();
        ImGui::SetNextItemWidth(ImGui::CalcTextSize("-Select Message Type").x * 1.3f);
        if (ImGui::BeginCombo("##1", MessageTypesStringArray[static_cast<int>(MidiDeviceProperty.MidiMessageType)]))
        {
            for (int i = 0; i < std::size(MessageTypesStringArray); i++)
            {
                if (ImGui::Selectable(MessageTypesStringArray[i]))
                {
                    MidiDeviceProperty.MidiMessageType = static_cast<IEMidiMessageType>(i);
                }
            }

            ImGui::EndCombo();
        }
        
        static const char ParameterTypesStringArray[static_cast<int>(IEMidiParameterType::Count)][std::size("-Select Parameter Type")] =
        {   "-Select Parameter Type",
            "Volume",
            "Mute",
            "Console Command",
            "Batch File" };
        ImGui::TableNextColumn();
        ImGui::SetNextItemWidth(ImGui::CalcTextSize("-Select Parameter Type").x * 1.3f);
        if (ImGui::BeginCombo("##2", ParameterTypesStringArray[static_cast<int>(MidiDeviceProperty.MidiParameterType)]))
        {
            for (int i = 0; i < std::size(ParameterTypesStringArray); i++)
            {
                if (ImGui::Selectable(ParameterTypesStringArray[i]))
                {
                    MidiDeviceProperty.MidiParameterType = static_cast<IEMidiParameterType>(i);
                }
            }

            ImGui::EndCombo();
        }

        switch (MidiDeviceProperty.MidiParameterType)
        {
            case IEMidiParameterType::ConsoleCommand:
            {
                ImGui::TableNextColumn();
                ImGui::TableSetColumnEnabled(-1, true);
                char Buffer[256] = {0}; 
                std::strncpy(Buffer, MidiDeviceProperty.ConsoleCommand.c_str(), sizeof(Buffer) - 1);
                Buffer[sizeof(Buffer) - 1] = '\0';
                ImGui::SetNextItemWidth(InputBoxSizeWidth);
                ImGui::InputText("##Input Console Command", Buffer, std::size(Buffer));
                MidiDeviceProperty.ConsoleCommand = Buffer;
                ImGui::TableNextColumn();
                ImGui::TableSetColumnEnabled(-1, false);
                MidiDeviceProperty.BatchFilePath.clear();
                break;
            }
            case IEMidiParameterType::BatchFile:
            {
                ImGui::TableNextColumn();
                ImGui::TableSetColumnEnabled(-1, false);
                MidiDeviceProperty.ConsoleCommand.clear();
                ImGui::TableNextColumn();
                ImGui::TableSetColumnEnabled(-1, true);
                char Buffer[256] = {0}; 
                std::strncpy(Buffer, MidiDeviceProperty.BatchFilePath.c_str(), sizeof(Buffer) - 1);
                Buffer[sizeof(Buffer) - 1] = '\0';
                ImGui::SetNextItemWidth(InputBoxSizeWidth);
                ImGui::InputText("##Input Batch File Path", Buffer, std::size(Buffer));
                MidiDeviceProperty.BatchFilePath = Buffer;

                ImGui::SameLine();
                DrawFileFinder(MidiDeviceProperty.BatchFilePath);
                break;
            }
            default:
            {
                ImGui::TableNextColumn();
                ImGui::TableSetColumnEnabled(-1, false);
                MidiDeviceProperty.ConsoleCommand.clear();

                ImGui::TableNextColumn();
                ImGui::TableSetColumnEnabled(-1, false);
                MidiDeviceProperty.BatchFilePath.clear();
                break;
            }
        }

        ImGui::TableNextColumn();
        ImGui::GetWindowDrawList()->ChannelsSplit(2);
        ImGui::GetWindowDrawList()->ChannelsSetCurrent(1);
        ImGui::Selectable("Record Midi", &MidiDeviceProperty.bIsRecording);
        ImGui::GetWindowDrawList()->ChannelsSetCurrent(0);
        ImGui::GetWindowDrawList()->AddRectFilled(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(255, 0, 0, 128));
        ImGui::GetWindowDrawList()->ChannelsMerge();

        ImGui::TableNextColumn();
        std::array<int, MIDI_MESSAGE_BYTE_COUNT> MidiMessageBuf = { MidiDeviceProperty.MidiMessage[0], MidiDeviceProperty.MidiMessage[1], MidiDeviceProperty.MidiMessage[2] };
        ImGui::SetNextItemWidth(InputBoxSizeWidth);
        ImGui::InputInt3("##Input Midi Message", MidiMessageBuf.data());
        MidiDeviceProperty.MidiMessage.resize(MIDI_MESSAGE_BYTE_COUNT);
        std::copy(MidiMessageBuf.begin(), MidiMessageBuf.end(), MidiDeviceProperty.MidiMessage.begin());

        ImGui::TableNextColumn();
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1, 0, 0, 0.2));
        bDeleteRequested = ImGui::Button("Delete") ? true : false;
        ImGui::PopStyleColor();

        ImGui::EndTable();
    }
}

void IEMidiEditor::DrawInitialOutputMessageEditor(std::vector<unsigned char>& MidiDeviceInitialOutputMidiMessage, bool& bDeleteRequested) const
{
    if (ImGui::BeginTable("Profile Midi Output Editor", MidiDeviceInitialOutputMessageEditorColumnCount, ImGuiTableFlags_SizingFixedFit))
    {
        ImGui::TableNextColumn();
        std::array<int, MIDI_MESSAGE_BYTE_COUNT> InitialOutputMidiMessageBuf = { MidiDeviceInitialOutputMidiMessage[0], MidiDeviceInitialOutputMidiMessage[1], MidiDeviceInitialOutputMidiMessage[2] };
        ImGui::SetNextItemWidth(InputBoxSizeWidth);
        ImGui::InputInt3("##Input Midi Message", InitialOutputMidiMessageBuf.data());
        MidiDeviceInitialOutputMidiMessage.resize(MIDI_MESSAGE_BYTE_COUNT);
        std::copy(InitialOutputMidiMessageBuf.begin(), InitialOutputMidiMessageBuf.end(), MidiDeviceInitialOutputMidiMessage.begin());

        ImGui::TableNextColumn();
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1, 0, 0, 0.2));
        bDeleteRequested = ImGui::Button("Delete") ? true : false;
        ImGui::PopStyleColor();

        ImGui::EndTable();
    }
}

void IEMidiEditor::DrawFileFinder(std::string& SelectedFile) const
{
    const char FileFinderPopupLabel[] = "File Finder";
    if (ImGui::Selectable("...", &m_bFileFinderOpen))
    {
        ImGui::OpenPopup(FileFinderPopupLabel);
    }

    static constexpr uint32_t WindowFlags = ImGuiWindowFlags_NoResize |
                                            ImGuiWindowFlags_NoMove |
                                            ImGuiWindowFlags_NoScrollbar |
                                            ImGuiWindowFlags_NoScrollWithMouse |
                                            ImGuiWindowFlags_NoCollapse;

    ImGuiIO& IO = ImGui::GetIO();
    const float WindowWidth = IO.DisplaySize.x * 0.5f;
    const float WindowHeight = IO.DisplaySize.y * 0.7f;

    const float WindowPosX = (IO.DisplaySize.x - WindowWidth) * 0.5f;
    const float WindowPosY = (IO.DisplaySize.y - WindowHeight) * 0.5f;

    ImGui::SetNextWindowSize(ImVec2(WindowWidth, WindowHeight), ImGuiCond_Always);
    ImGui::SetNextWindowPos(ImVec2(WindowPosX, WindowPosY));

    if (ImGui::BeginPopupModal(FileFinderPopupLabel, &m_bFileFinderOpen, WindowFlags))
    {
        ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.0f, 0.5f));
        if (ImGui::Selectable("...##Inner", false, ImGuiSelectableFlags_DontClosePopups))
        {
            m_RootFileFinderSearchPath = m_RootFileFinderSearchPath.parent_path();
        }
        ImGui::SameLine();
        ImGui::Text(m_RootFileFinderSearchPath.string().c_str());
        ImGui::PopStyleVar();

        DrawFileFinderTree(m_RootFileFinderSearchPath, 3, SelectedFile);
        ImGui::EndPopup();
    }
}

void IEMidiEditor::DrawFileFinderTree(const std::filesystem::path& CurrentPath, uint8_t Depth, std::string& SelectedFile) const
{
    if (!CurrentPath.empty())
    {
        if (Depth == 0)
        {
            m_RootFileFinderSearchPath = CurrentPath;
            ImGui::ClearCurrentWindowStoredStates();
            return;
        }

        for (std::filesystem::directory_iterator It = std::filesystem::directory_iterator(CurrentPath);
            It != std::filesystem::directory_iterator(); It++)
        {
            const std::filesystem::path& SubPath = *It;

            if (!IEUtils::IsFileHidden(SubPath))
            {
                if (std::filesystem::is_directory(SubPath))
                {
                    if (ImGui::TreeNodeEx(SubPath.filename().string().c_str()))
                    {
                        DrawFileFinderTree(SubPath, Depth - 1, SelectedFile);
                        ImGui::TreePop();
                    }
                }
                else
                {
                    ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.0f, 0.5f));
                    if (ImGui::Selectable(SubPath.filename().string().c_str()))
                    {
                        SelectedFile = SubPath.string();
                    }
                    ImGui::PopStyleVar();
                }   
            }
        }
    }
}