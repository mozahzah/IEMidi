// Copyright © 2024 mozahzah (Incus Entertainment). All rights reserved.

#include "IEMidiMapper.h"
#include "IEUtils.h"

#include "IEExtensions/imgui_IE.h"

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
                IO.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_IsSRGB;
                ImGui::StyleColorsIE();

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

                    
                    ImGui::ShowDemoWindow();
                    ImGui::Begin("Window!");

                    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / IO.Framerate, IO.Framerate);

                    const uint32_t ObservedFrameTimeMs = CapturedDelta.count() * 1000.0f;
                    const double ObservedFrameRate = 1.0f / CapturedDelta.count();
                    ImGui::Text("Observed frame time %.2f ms/frame (%.0f FPS)", ObservedFrameTimeMs, ObservedFrameRate);

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