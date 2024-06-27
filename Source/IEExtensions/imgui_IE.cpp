// Copyright © 2024 mozahzah (Incus Entertainment). All rights reserved.

#include "IEExtensions/imgui_IE.h"

#include "imgui_internal.h"

#include "IECore.h"
#include "IEUtils.h"

namespace ImGui
{
    void StyleIE(ImGuiStyle* StyleDestination)
    {
        ImGuiIO& IO = ImGui::GetIO();
        IO.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_IsSRGB;

        const std::filesystem::path AppDirectory = IEUtils::FindFolderPathUpwards(std::filesystem::current_path(), "IEMidi");

        const std::filesystem::path ImGuiIniFilePath = AppDirectory / "Settings/IEMidi_ImGui.ini";
        const std::string ImGuiIniFilePathString = IEUtils::StringCast<char>(ImGuiIniFilePath.c_str());
        ImGui::LoadIniSettingsFromDisk(ImGuiIniFilePathString.c_str());
        IO.IniFilename = ImGuiIniFilePathString.c_str();
        
        const std::filesystem::path RobotoMonoFontPath = AppDirectory / "Resources/Fonts/Roboto_Mono/static/RobotoMono-Medium.ttf";
        if (ImFont* const RobotoMonoFont = IO.Fonts->AddFontFromFileTTF(IEUtils::StringCast<char>(RobotoMonoFontPath.c_str()).c_str(), 22.0f))
        {
            IO.Fonts->Build();
            IELOG_SUCCESS("Successfully loaded font (%s)", RobotoMonoFont->GetDebugName());
        }

        if (ImGuiStyle* const Style = StyleDestination ? StyleDestination : &ImGui::GetStyle())
        {
            Style->FrameRounding = 2.0f;
            Style->WindowRounding = 2.0f;
            Style->WindowBorderSize = 1.0f;
            Style->WindowPadding = ImVec2(5.0f, 5.0f);
            Style->FramePadding = ImVec2(5.0f, 5.0f);
            Style->CellPadding = ImVec2(5.0f, 5.0f);
            Style->ItemSpacing = ImVec2(6.0f, 6.0f);
            Style->ButtonTextAlign = ImVec2(0.5f, 0.5f);
            Style->WindowTitleAlign = ImVec2(0.5f, 0.5f);

            if (ImVec4* const Colors = Style->Colors)
            {
                Colors[ImGuiCol_Text]                   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
                Colors[ImGuiCol_TextDisabled]           = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
                Colors[ImGuiCol_WindowBg]               = ImVec4(1.00f, 1.00f, 1.00f, 0.00f);
                Colors[ImGuiCol_ChildBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.71f);
                Colors[ImGuiCol_PopupBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.71f);
                Colors[ImGuiCol_Border]                 = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
                Colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
                Colors[ImGuiCol_FrameBg]                = ImVec4(0.13f, 0.13f, 0.13f, 0.73f);
                Colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
                Colors[ImGuiCol_FrameBgActive]          = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
                Colors[ImGuiCol_TitleBg]                = ImVec4(0.35f, 0.35f, 0.35f, 0.78f);
                Colors[ImGuiCol_TitleBgActive]          = ImVec4(0.35f, 0.35f, 0.35f, 0.78f);
                Colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.35f, 0.35f, 0.35f, 0.78f);
                Colors[ImGuiCol_MenuBarBg]              = ImVec4(0.35f, 0.35f, 0.35f, 0.63f);
                Colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
                Colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
                Colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
                Colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
                Colors[ImGuiCol_CheckMark]              = ImVec4(0.66f, 0.26f, 0.98f, 0.63f);
                Colors[ImGuiCol_SliderGrab]             = ImVec4(0.66f, 0.26f, 0.98f, 0.63f);
                Colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.66f, 0.26f, 0.98f, 0.79f);
                Colors[ImGuiCol_Button]                 = ImVec4(0.15f, 0.48f, 0.74f, 0.50f);
                Colors[ImGuiCol_ButtonHovered]          = ImVec4(0.66f, 0.26f, 0.98f, 0.78f);
                Colors[ImGuiCol_ButtonActive]           = ImVec4(0.66f, 0.26f, 0.98f, 0.78f);
                Colors[ImGuiCol_Header]                 = ImVec4(0.67f, 0.67f, 0.67f, 0.20f);
                Colors[ImGuiCol_HeaderHovered]          = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
                Colors[ImGuiCol_HeaderActive]           = ImVec4(0.66f, 0.66f, 0.66f, 0.78f);
                Colors[ImGuiCol_Separator]              = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
                Colors[ImGuiCol_SeparatorHovered]       = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
                Colors[ImGuiCol_SeparatorActive]        = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
                Colors[ImGuiCol_ResizeGrip]             = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
                Colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.67f, 0.67f, 0.67f, 1.00f);
                Colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.67f, 0.67f, 0.67f, 1.00f);
                Colors[ImGuiCol_TabHovered]             = ImVec4(0.18f, 0.18f, 0.18f, 0.78f);
                Colors[ImGuiCol_Tab]                    = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
                Colors[ImGuiCol_TabSelected]            = ImVec4(0.66f, 0.26f, 0.98f, 0.63f);
                Colors[ImGuiCol_TabSelectedOverline]    = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
                Colors[ImGuiCol_TabDimmed]              = ImVec4(0.07f, 0.10f, 0.15f, 0.97f);
                Colors[ImGuiCol_TabDimmedSelected]      = ImVec4(0.14f, 0.26f, 0.42f, 1.00f);
                Colors[ImGuiCol_TabDimmedSelectedOverline]  = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
                Colors[ImGuiCol_PlotLines]              = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
                Colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
                Colors[ImGuiCol_PlotHistogram]          = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
                Colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
                Colors[ImGuiCol_TableHeaderBg]          = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
                Colors[ImGuiCol_TableBorderStrong]      = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
                Colors[ImGuiCol_TableBorderLight]       = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
                Colors[ImGuiCol_TableRowBg]             = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
                Colors[ImGuiCol_TableRowBgAlt]          = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
                Colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
                Colors[ImGuiCol_DragDropTarget]         = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
                Colors[ImGuiCol_NavHighlight]           = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
                Colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
                Colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
                Colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
            }
        }
    }
}