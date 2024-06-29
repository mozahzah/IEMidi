// Copyright © 2024 mozahzah (Incus Entertainment). All rights reserved.

#include "IEMidi.h"

#include "IEExtensions/imgui_IE.h"

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
                ImGui::StyleColorsDark();
                IEMidiApp.SetAppState(IEAppState::MidiDeviceSelection);

                IEClock::time_point StartFrameTime = IEClock::now();
                IEDurationMs CapturedDeltaTime = IEDurationMs::zero();
                
                while (Renderer.IsAppWindowOpen())
                {
                    StartFrameTime = IEClock::now();

                    Renderer.PollEvents();
                    Renderer.CheckAndResizeSwapChain();
                    Renderer.NewFrame();
                    ImGui::NewFrame();

                    IEMidiApp.OnPreFrameRender();
                    
                    Renderer.DrawTelemetry();
                        
                    ImGui::Render();
                    Renderer.RenderFrame(*ImGui::GetDrawData());
                    Renderer.PresentFrame();

                    IEMidiApp.OnPostFrameRender();

                    CapturedDeltaTime = std::chrono::duration_cast<IEDurationMs>(IEClock::now() - StartFrameTime);
                    const int32_t SleepDurationMs = TARGET_FRAME_DURATION - CapturedDeltaTime.count();
                    if (SleepDurationMs > 1)
                    {
                        std::this_thread::sleep_for(std::chrono::milliseconds(SleepDurationMs));
                    }
                }
            }
        }

        Renderer.Deinitialize();
    }

    return 0;
}