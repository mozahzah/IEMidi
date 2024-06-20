// Copyright © 2024 mozahzah (Incus Entertainment). All rights reserved.

#include <cstdlib>
#include <comdef.h>

#include "imgui.h"

#include "IEMidiMapper.h"
#include "IEUtils.h"

int main(int, char**)
{
    // Init
    IEMidiMapper IEMidiMapperApp;

    std::filesystem::path ImGuiIniFile = std::filesystem::current_path().parent_path() / "Settings/IEMidiMapper_ImGui.ini";
    ImGui::LoadIniSettingsFromDisk(IEUtils::ConvertToChar(ImGuiIniFile.c_str()).c_str());

    IERenderer& Renderer = IEMidiMapperApp.GetRenderer();
    Renderer.Initialize();

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    std::filesystem::path UbuntuMonoFontPath = std::filesystem::current_path().parent_path() / "Resources/Fonts/Roboto_Mono/static/RobotoMono-Medium.ttf";
    ImFont *UbuntuMonoFont = io.Fonts->AddFontFromFileTTF((char*)UbuntuMonoFontPath.c_str(), 22.0f);
    io.Fonts->Build();

  
    
    // Main loop
    while (!Renderer.IsAppWindowClosed())
    {
        // poll events
        Renderer.PollEvents();
    
        // Start the Dear ImGui frame
        Renderer.NewFrame();
        ImGui::NewFrame();

        ImGui::PopFont();

        // Rendering
        ImGui::Render();
        Renderer.RenderFrame(*ImGui::GetDrawData());
        Renderer.PresentFrame();
    }

    Renderer.Deinitialize();
    ImGui::DestroyContext();
    Renderer.Cleanup();

    return 0;
}