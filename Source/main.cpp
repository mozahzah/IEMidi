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
            if (Renderer.PostImGuiContextCreated())
            {
                ImGui::StyleIE();
                ImGuiIO& IO = ImGui::GetIO();
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

                    //ImGui::ShowDemoWindow();
                    IEMidiApp.OnPreFrameRender();

                    Renderer.DrawTelemetry();
                        
                    ImGui::Render();
                    Renderer.RenderFrame(*ImGui::GetDrawData());
                    Renderer.PresentFrame();

                    IEMidiApp.OnPostFrameRender();

                    CapturedDeltaTime = std::chrono::duration_cast<IEDurationMs>(IEClock::now() - StartFrameTime);
                    Renderer.WaitEvents();
                }
            }
        }

        Renderer.Deinitialize();
    }

    return 0;
}