// Copyright © 2024 mozahzah (Incus Entertainment). All rights reserved.

#include "IEMidiEditor.h"

#include "imgui.h"

static const char* IEMidiMessageTypeStringArray[]{ "-Select Message Type", "NoteOnOff", "ControlChange" };

void IEMidiEditor::DrawMidiDeviceProfileEditor(IEMidiDeviceProfile& MidiDeviceProfile) const
{
    if (ImGui::BeginTable("Profile Midi Editor", 4))
    {
        DrawMidiDevicePropertyEditor(MidiDeviceProfile.Properties);
        ImGui::TableNextColumn();
        if (ImGui::Button("+", ImVec2(20, 20)))
        {
            MidiDeviceProfile.Properties.push_back(IEMidiDeviceProperty());
        }
        ImGui::EndTable();
    }

    if (ImGui::BeginTable("Output Midi Editor", 2))
    {
        for (std::vector<unsigned char>& InitialOutputMidiMessage : MidiDeviceProfile.InitialOutputMidiMessages)
        {
            ImGui::PushID(&InitialOutputMidiMessage);

            ImGui::TableNextColumn();
            ImGui::Text("Initial output midi message");
            ImGui::TableNextColumn();
            int InitialOutputMidiMessageBuf[3] = {InitialOutputMidiMessage[0], InitialOutputMidiMessage[1], InitialOutputMidiMessage[2]};
            ImGui::InputInt3("Initial output midi message", InitialOutputMidiMessageBuf);

            InitialOutputMidiMessage.resize(3);
            for (int i = 0; i < 3; i++)
            {
                InitialOutputMidiMessage[i] = InitialOutputMidiMessageBuf[i];
            }

            ImGui::PopID();
        }

        ImGui::TableNextColumn();
        if (ImGui::Button("+", ImVec2(20, 20)))
        {
            MidiDeviceProfile.InitialOutputMidiMessages.push_back(std::vector<unsigned char>(3));
        }
        ImGui::EndTable();
    }
}

void IEMidiEditor::DrawMidiDevicePropertyEditor(std::vector<IEMidiDeviceProperty>& MidiDeviceProperties) const
{
    if (IEAssert(ImGui::TableGetColumnCount() >= 3))
    {
        for (std::vector<IEMidiDeviceProperty>::iterator It = MidiDeviceProperties.begin(); It != MidiDeviceProperties.end();)
        {
            IEMidiDeviceProperty& MidiDeviceProperty = *It;
            ImGui::PushID(It.base());
            
            ImGui::TableNextColumn();
            ImGui::Combo("MidiMessageType", (int*)(&MidiDeviceProperty.MidiMessageType), IEMidiMessageTypeStringArray, std::size(IEMidiMessageTypeStringArray));

            char Buff[256] = {};
            ImGui::TableNextColumn();ImGui::InputText("Type", Buff, std::size(Buff));
            ImGui::TableNextColumn();
            if (ImGui::Selectable("Record", &MidiDeviceProperty.bIsRecording, 0, ImVec2(50, 20)))
            {
                // RECORD FIRST MIDI Message
                // UNSELECT i.e MidiDeviceProperty.bIsRecording = false;
            }

            ImGui::TableNextColumn();
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1, 0, 0, 0.2));
            It = ImGui::Button("Delete") ? MidiDeviceProperties.erase(It) : It + 1;
            ImGui::PopStyleColor();

            ImGui::PopID();
        }
    }
    else
    {
        IELOG_ERROR("DrawMidiDevicePropertyEditor must be called inside a Table with 3 or more columns.");
    }
}
