// Copyright © 2024 mozahzah (Incus Entertainment). All rights reserved.

#include "imgui.h"

#include "IEMidiMapper.h"
#include "IEUtils.h"

int main()
{
    IEMidiMapper IEMidiMapperApp;
    IERenderer& Renderer = IEMidiMapperApp.GetRenderer();
    if (Renderer.Initialize())
    {
        if (ImGuiContext* const CreatedImGuiContext = ImGui::CreateContext())
        {
            if (Renderer.PostImGuiContextCreated())
            {
                ImGuiIO& IO = ImGui::GetIO();
                IO.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
                ImGui::StyleColorsDark();

                const std::filesystem::path AppDirectory = IEUtils::FindFolderPathUpwards(std::filesystem::current_path(), "IEMidiMapper");

                const std::filesystem::path ImGuiIniFilePath = AppDirectory / "Settings/IEMidiMapper_ImGui.ini";
                ImGui::LoadIniSettingsFromDisk(IEUtils::StringCast<char>(ImGuiIniFilePath.c_str()).c_str());

                const std::filesystem::path RobotoMonoFontPath = AppDirectory / "Resources/Fonts/Roboto_Mono/static/RobotoMono-Medium.ttf";
                if (ImFont* const RobotoMonoFont = IO.Fonts->AddFontFromFileTTF(IEUtils::StringCast<char>(RobotoMonoFontPath.c_str()).c_str(), 22.0f))
                {
                    IO.Fonts->Build();
                    IELOG_SUCCESS("Successfully loaded font (%s)", RobotoMonoFont->GetDebugName());
                }

                IEDuration CapturedDelta;
                IEClock::time_point Timer = IEClock::now();
                while (Renderer.IsAppWindowOpen())
                {
                    Renderer.PollEvents();

                    Renderer.CheckAndResizeSwapChain();

                    Renderer.NewFrame();
                    ImGui::NewFrame();

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

                    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / IO.Framerate, IO.Framerate);
                    ImGui::Text("Observed frame time %.2f ms/frame (%.0f FPS)", CapturedDelta.count() * 1000.0f, 1.0f / CapturedDelta.count());
                    
                    ImGui::End();
                    
                    // Rendering
                    ImGui::Render();
                    Renderer.RenderFrame(*ImGui::GetDrawData());
                    Renderer.PresentFrame();

                    // Measure time at the end of the frame
                    IEClock::time_point Now = IEClock::now();
                    CapturedDelta = std::chrono::duration_cast<IEDuration>(Now - Timer);
                    Timer = Now;
                }
            }
        }
        Renderer.Deinitialize();
    }

    return 0;
}