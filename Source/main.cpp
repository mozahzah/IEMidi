// Copyright © 2024 mozahzah (Incus Entertainment). All rights reserved.

#include "IEMidi.h"

#include "IEExtensions/ie.imgui.h"

int main()
{
    IEMidi IEMidiApp;
    IEMidiApp.SetAppState(IEAppState::Loading);

    IERenderer& Renderer = IEMidiApp.GetRenderer();
    if (Renderer.Initialize())
    {
        if (ImGuiContext* const CreatedImGuiContext = ImGui::CreateContext())
        {
            ImGuiIO& IO = ImGui::GetIO();
            IO.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_IsSRGB | ImGuiConfigFlags_ViewportsEnable;
            if (Renderer.PostImGuiContextCreated())
            {
                ImGui::IEStyle::StyleIE();
                IO.IniFilename = nullptr;
                IO.LogFilename = nullptr;

                IEMidiApp.SetAppState(IEAppState::MidiDeviceSelection);

                IEClock::time_point StartFrameTime = IEClock::now();
                IEDurationMs CapturedDeltaTime = IEDurationMs::zero();

                while (Renderer.IsAppRunning())
                {
                    StartFrameTime = IEClock::now();

                    Renderer.CheckAndResizeSwapChain();
                    Renderer.NewFrame();
                    ImGui::NewFrame();

                    IEMidiApp.OnPreFrameRender();
                        
                    ImGui::Render();
                    Renderer.RenderFrame(*ImGui::GetDrawData());
                    if (IO.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
                    {
                        ImGui::UpdatePlatformWindows();
                        ImGui::RenderPlatformWindowsDefault();
                    }
                    Renderer.PresentFrame();

                    IEMidiApp.OnPostFrameRender();

                    CapturedDeltaTime = std::chrono::duration_cast<IEDurationMs>(IEClock::now() - StartFrameTime);
                    if (Renderer.IsAppWindowOpen())
                    {
                        Renderer.WaitEventsTimeout(0.1f);
                    }
                    else
                    {
                        Renderer.WaitEvents();
                    }
                }
            }
        }

        Renderer.Deinitialize();
    }

    return 0;
}