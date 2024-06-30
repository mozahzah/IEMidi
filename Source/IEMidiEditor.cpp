// Copyright © 2024 mozahzah (Incus Entertainment). All rights reserved.

#include "IEMidiEditor.h"

#include "imgui.h"

namespace IEMidiEditorStyles
{
    static constexpr size_t MidiDevicePropertyEditorColumnCount = 5;
    static constexpr size_t MidiDeviceInitialOutputMessageEditorColumnCount = 2;
    static constexpr float MidiMessageInputBoxSizeX = 100.0f;
}

void IEMidiEditor::DrawMidiDeviceProfileEditor(IEMidiDeviceProfile& MidiDeviceProfile) const
{
    static const ImVec2 PlusButtonSize = ImVec2(ImGui::GetFontSize() * 2.0f, ImGui::GetFontSize() * 2.0f);

    if (ImGui::BeginTable("Profile Midi Input Editor", IEMidiEditorStyles::MidiDevicePropertyEditorColumnCount))
    {
        ImGui::TableSetupColumn("##1", ImGuiTableColumnFlags_WidthFixed, ImGui::CalcTextSize("-Select Message Type").x * 1.5);
        ImGui::TableSetupColumn("##2", ImGuiTableColumnFlags_WidthFixed, ImGui::CalcTextSize("-Select Parameter Type").x * 1.5);
        ImGui::TableSetupColumn("##3", ImGuiTableColumnFlags_WidthFixed, ImGui::CalcTextSize("Record Midi").x);
        ImGui::TableSetupColumn("##4", ImGuiTableColumnFlags_WidthFixed, 200.0f);
        ImGui::TableSetupColumn("##5", ImGuiTableColumnFlags_WidthFixed, ImGui::CalcTextSize("Delete").x * 1.5);

        for (std::vector<IEMidiDeviceProperty>::iterator It = MidiDeviceProfile.Properties.begin();
            It != MidiDeviceProfile.Properties.end();)
        {
            ImGui::PushID(It.base());

            DrawMidiDeviceInputPropertyEditor(*It);

            ImGui::TableNextColumn();
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1, 0, 0, 0.2));
            It = ImGui::Button("Delete") ? MidiDeviceProfile.Properties.erase(It) : It + 1;
            ImGui::PopStyleColor();

            ImGui::PopID();
        }

        ImGui::TableNextColumn();
        if (ImGui::Button("+", PlusButtonSize))
        {
            MidiDeviceProfile.Properties.push_back(IEMidiDeviceProperty());
        }

        ImGui::EndTable();
    }
    MidiDeviceProfile.Properties.shrink_to_fit();

    if (ImGui::BeginTable("Profile Output Midi Message Editor", IEMidiEditorStyles::MidiDeviceInitialOutputMessageEditorColumnCount))
    {
        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 200);
        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, ImGui::CalcTextSize("Delete").x * 1.5);

        for (std::vector<std::vector<unsigned char>>::iterator It = MidiDeviceProfile.InitialOutputMidiMessages.begin();
            It != MidiDeviceProfile.InitialOutputMidiMessages.end();)
        {
            ImGui::PushID(It.base());

            DrawInitialOutputMessageEditor(*It);

            ImGui::TableNextColumn();
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1, 0, 0, 0.2));
            It = ImGui::Button("Delete") ? MidiDeviceProfile.InitialOutputMidiMessages.erase(It) : It + 1;
            ImGui::PopStyleColor();

            ImGui::PopID();
        }
        
        ImGui::TableNextColumn();
        if (ImGui::Button("+", PlusButtonSize))
        {
            MidiDeviceProfile.InitialOutputMidiMessages.push_back(std::vector<unsigned char>(MIDI_MESSAGE_BYTE_COUNT));
        }

        ImGui::EndTable();
    }
    MidiDeviceProfile.InitialOutputMidiMessages.shrink_to_fit();
}

void IEMidiEditor::DrawMidiDeviceInputPropertyEditor(IEMidiDeviceProperty& MidiDeviceProperty) const
{
    if (IEAssert(ImGui::TableGetColumnCount() == IEMidiEditorStyles::MidiDevicePropertyEditorColumnCount))
    {
        ImGui::TableNextColumn();
        static const char MessageTypesStringArray[static_cast<int>(IEMidiMessageType::Count)][std::size("-Select Message Type")] =
        {   "-Select Message Type",
            "NoteOnOff",
            "ControlChange" };

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
        
        ImGui::TableNextColumn();
        static const char ParameterTypesStringArray[static_cast<int>(IEMidiParameterType::Count)][std::size("-Select Parameter Type")] =
        {   "-Select Parameter Type",
            "Volume",
            "Mute",
            "Console Command",
            "Batch File" };

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
        
        ImGui::TableNextColumn();
        ImGui::SetNextItemWidth(ImGui::CalcTextSize("Record Midi").x);
        ImGui::Selectable("Record Midi", &MidiDeviceProperty.bIsRecording, 0);

        ImGui::TableNextColumn();
        std::array<int, MIDI_MESSAGE_BYTE_COUNT> MidiMessageBuf = { MidiDeviceProperty.MidiMessage[0], MidiDeviceProperty.MidiMessage[1], MidiDeviceProperty.MidiMessage[2] };
        ImGui::InputInt3("##Midi Message", MidiMessageBuf.data());
        MidiDeviceProperty.MidiMessage.resize(MIDI_MESSAGE_BYTE_COUNT);
        std::copy(MidiMessageBuf.begin(), MidiMessageBuf.end(), MidiDeviceProperty.MidiMessage.begin());
    }
}

void IEMidiEditor::DrawInitialOutputMessageEditor(std::vector<unsigned char>& MidiDeviceInitialOutputMidiMessage) const
{
    if (IEAssert(ImGui::TableGetColumnCount() == IEMidiEditorStyles::MidiDeviceInitialOutputMessageEditorColumnCount))
    {
        ImGui::TableNextColumn();
        std::array<int, MIDI_MESSAGE_BYTE_COUNT> InitialOutputMidiMessageBuf = { MidiDeviceInitialOutputMidiMessage[0], MidiDeviceInitialOutputMidiMessage[1], MidiDeviceInitialOutputMidiMessage[2] };
        ImGui::InputInt3("", InitialOutputMidiMessageBuf.data());
        MidiDeviceInitialOutputMidiMessage.resize(MIDI_MESSAGE_BYTE_COUNT);
        std::copy(InitialOutputMidiMessageBuf.begin(), InitialOutputMidiMessageBuf.end(), MidiDeviceInitialOutputMidiMessage.begin());
    }
}
