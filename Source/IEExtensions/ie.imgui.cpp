// Copyright © 2024 mozahzah (Incus Entertainment). All rights reserved.

#include "IEExtensions/ie.imgui.h"

#include "IECore/IECore.h"
#include "IECore/IEUtils.h"

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

        const std::filesystem::path RobotoMonoFontPath = AppDirectory / "Resources/Fonts/Roboto_Mono/static/RobotoMono-Thin.ttf";
        if (ImFont* const RobotoMonoFont = IO.Fonts->AddFontFromFileTTF(IEUtils::StringCast<char>(RobotoMonoFontPath.c_str()).c_str(), 30.0f))
        {
            IO.Fonts->Build();
            IO.FontGlobalScale = 0.7f;
            IELOG_SUCCESS("Successfully loaded font (%s)", RobotoMonoFont->GetDebugName());
        }

        if (ImGuiStyle* const Style = StyleDestination ? StyleDestination : &ImGui::GetStyle())
        {
            /* Main */
            Style->WindowPadding = ImVec2(10.0f, 10.0f);
            Style->FramePadding = ImVec2(5.0f, 5.0f);
            Style->ItemSpacing = ImVec2(8.0f, 8.0f);
            Style->ItemInnerSpacing = ImVec2(8.0f, 8.0f);
            Style->IndentSpacing = 15.0f;
            Style->ScrollbarSize = 15.0f;
            Style->GrabMinSize = 10.0f;

            /* Borders */
            Style->WindowBorderSize = 0.0f;
            Style->ChildBorderSize = 0.0f;
            Style->PopupBorderSize = 1.0f;
            Style->FrameBorderSize = 0.0f;
            Style->TabBorderSize = 0.0f;
            Style->TabBarBorderSize = 0.0f;

            /* Rounding */
            Style->WindowRounding = 1.0f;
            Style->ChildRounding = 1.0f;
            Style->FrameRounding = 2.0f;
            Style->PopupRounding = 2.0f;
            Style->ScrollbarRounding = 12.0f;
            Style->GrabRounding = 12.0f;
            Style->TabRounding = 2.0f;

            /* Tables */
            Style->CellPadding = ImVec2(7.0f, 7.0f);
            Style->TableAngledHeadersAngle = 35.0f;
            Style->TableAngledHeadersTextAlign = ImVec2(0.5f, 0.0f);

            /* Widgets */
            Style->WindowTitleAlign = ImVec2(0.5f, 0.5f);
            Style->WindowMenuButtonPosition = ImGuiDir_Left;
            Style->ColorButtonPosition = ImGuiDir_Left;
            Style->ButtonTextAlign = ImVec2(0.5f, 0.5f);
            Style->SelectableTextAlign = ImVec2(0.5f, 0.5f);
            Style->SeparatorTextBorderSize = 5.0f;
            Style->SeparatorTextAlign = ImVec2(0.5f, 0.5f);
            Style->SeparatorTextPadding = ImVec2(20.0f, 4.0f);

            if (ImVec4* const Colors = Style->Colors)
            {
                Colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
                Colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
                Colors[ImGuiCol_WindowBg] = ImVec4(0.1255f, 0.1255f, 0.1255f, 1.0f);
                Colors[ImGuiCol_ChildBg] = ImVec4(0.1255f, 0.1255f, 0.1255f, 1.0f);
                Colors[ImGuiCol_PopupBg] = ImVec4(0.1255f, 0.1255f, 0.1255f, 1.0f);
                Colors[ImGuiCol_Border] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
                Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
                Colors[ImGuiCol_FrameBg] = ImVec4(0.13f, 0.13f, 0.13f, 0.73f);
                Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
                Colors[ImGuiCol_FrameBgActive] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
                Colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
                Colors[ImGuiCol_TitleBgActive] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
                Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
                Colors[ImGuiCol_MenuBarBg] = ImVec4(0.35f, 0.35f, 0.35f, 0.63f);
                Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
                Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
                Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
                Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
                Colors[ImGuiCol_CheckMark] = ImVec4(0.66f, 0.26f, 0.98f, 0.63f);
                Colors[ImGuiCol_SliderGrab] = ImVec4(0.66f, 0.26f, 0.98f, 0.63f);
                Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.66f, 0.26f, 0.98f, 0.79f);
                Colors[ImGuiCol_Button] = ImVec4(1.00f, 0.60f, 1.00f, 0.50f);
                Colors[ImGuiCol_ButtonHovered] = ImVec4(0.66f, 0.26f, 0.98f, 0.78f);
                Colors[ImGuiCol_ButtonActive] = ImVec4(0.66f, 0.26f, 0.98f, 0.78f);
                Colors[ImGuiCol_Header] = ImVec4(0.67f, 0.67f, 0.67f, 0.20f);
                Colors[ImGuiCol_HeaderHovered] = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
                Colors[ImGuiCol_HeaderActive] = ImVec4(0.66f, 0.66f, 0.66f, 0.78f);
                Colors[ImGuiCol_Separator] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
                Colors[ImGuiCol_SeparatorHovered] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
                Colors[ImGuiCol_SeparatorActive] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
                Colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
                Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.67f, 0.67f, 0.67f, 1.00f);
                Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.67f, 0.67f, 0.67f, 1.00f);
                Colors[ImGuiCol_TabHovered] = ImVec4(0.18f, 0.18f, 0.18f, 0.78f);
                Colors[ImGuiCol_Tab] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
                Colors[ImGuiCol_TabSelected] = ImVec4(0.66f, 0.26f, 0.98f, 0.63f);
                Colors[ImGuiCol_TabSelectedOverline] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
                Colors[ImGuiCol_TabDimmed] = ImVec4(0.07f, 0.10f, 0.15f, 0.97f);
                Colors[ImGuiCol_TabDimmedSelected] = ImVec4(0.14f, 0.26f, 0.42f, 1.00f);
                Colors[ImGuiCol_TabDimmedSelectedOverline] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
                Colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
                Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
                Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
                Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
                Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
                Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
                Colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
                Colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
                Colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
                Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
                Colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
                Colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
                Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
                Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
                Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
            }
        }
    }

    void FileFinder(const char* Label, int Depth, std::string& SelectedFile)
    {
        static bool bFileFinderOpen = false;
        static std::filesystem::path RootFileFinderSearchPath = std::filesystem::current_path();

        auto DrawFileFinderTree = [](const std::filesystem::path& CurrentPath, int Depth, std::string& SelectedFile, auto&& DrawFileFinderTree)
            {
                if (!CurrentPath.empty())
                {
                    if (Depth <= 0)
                    {
                        RootFileFinderSearchPath = CurrentPath;
                        ImGui::ClearCurrentWindowStoredStates();
                        return;
                    }

                    for (const std::filesystem::directory_entry& Entry : std::filesystem::directory_iterator(CurrentPath))
                    {
                        const std::filesystem::path& SubPath = Entry.path();
                        if (!IEUtils::IsFileHidden(SubPath))
                        {
                            if (std::filesystem::is_directory(SubPath))
                            {
                                if (ImGui::TreeNodeEx(SubPath.filename().string().c_str()))
                                {
                                    DrawFileFinderTree(SubPath, Depth - 1, SelectedFile, DrawFileFinderTree);
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
            };

        const char* FileFinderPopupLabel = Label ? Label : "File Finder";
        if (ImGui::Selectable("...##Outer", &bFileFinderOpen))
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

        if (ImGui::BeginPopupModal(FileFinderPopupLabel, &bFileFinderOpen, WindowFlags))
        {
            ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.0f, 0.5f));
            if (ImGui::Selectable("...", false, ImGuiSelectableFlags_DontClosePopups, ImVec2(ImGui::CalcTextSize("..."))))
            {
                RootFileFinderSearchPath = RootFileFinderSearchPath.parent_path();
            }
            ImGui::SameLine();
            ImGui::Text("%s", RootFileFinderSearchPath.string().c_str());
            ImGui::PopStyleVar();

            DrawFileFinderTree(RootFileFinderSearchPath, 3, SelectedFile, DrawFileFinderTree);

            ImGui::EndPopup();
        }
    }

    void ClearCurrentWindowStoredStates()
    {
        if (const ImGuiContext* const CurrentContext = ImGui::GetCurrentContext())
        {
            if (const ImGuiWindow* const CurrentWindow = CurrentContext->CurrentWindow)
            {
                const ImGuiWindowTempData CurrentWindowTempData = CurrentWindow->DC;
                if (ImGuiStorage* const CurrentWindowStateStorage = CurrentWindowTempData.StateStorage)
                {
                    CurrentWindowStateStorage->Clear();
                }
            }
        }
    }
}