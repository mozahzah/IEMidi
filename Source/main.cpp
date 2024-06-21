// Copyright © 2024 mozahzah (Incus Entertainment). All rights reserved.

#include "imgui.h"

#include "IEMidiMapper.h"
#include "IEUtils.h"

int main()
{
    // Init
    IEMidiMapper IEMidiMapperApp;
    IERenderer& Renderer = IEMidiMapperApp.GetRenderer();
    if (!Renderer.Initialize())
    {
        return 1;
    }

    // Setup Dear ImGui context
    //IMGUI_CHECKVERSION();
    //ImGui::CreateContext();

    const std::filesystem::path ImGuiIniFile = std::filesystem::current_path().parent_path() / "Settings/IEMidiMapper_ImGui.ini";
    ImGui::LoadIniSettingsFromDisk(IEUtils::StringCast<char>(ImGuiIniFile.c_str()).c_str());

    std::filesystem::path UbuntuMonoFontPath = std::filesystem::current_path().parent_path() / "Resources/Fonts/Roboto_Mono/static/RobotoMono-Medium.ttf";
    //ImFont *UbuntuMonoFont = io.Fonts->AddFontFromFileTTF((char*)UbuntuMonoFontPath.c_str(), 22.0f);
    ImGui::GetIO().Fonts->Build();

    // Main loop
    while (!Renderer.IsAppWindowClosed())
    {
        // poll events
        Renderer.PollEvents();

        Renderer.CheckAndResizeSwapChain();
        // Start the Dear ImGui frame
        Renderer.NewFrame();
        ImGui::NewFrame();

        {
            static float f = 0.0f;
            static int counter = 0;
            bool show_demo_window;

            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            //ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            ImGui::End();
        }

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