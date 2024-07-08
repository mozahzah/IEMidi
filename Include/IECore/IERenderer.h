// Copyright © 2024 mozahzah (Incus Entertainment). All rights reserved.

#pragma once

#define GLFW_INCLUDE_VULKAN
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

#if defined (_WIN32)
#define GLFW_EXPOSE_NATIVE_WIN32 1
#endif

#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

#include "IECore/IECore.h"

class IERenderer
{
public:
    virtual ~IERenderer() = default;

public:
    virtual IEResult Initialize() = 0;
    virtual IEResult PostImGuiContextCreated() = 0;
    virtual void Deinitialize() = 0;
    virtual int32_t FlushGPUCommandsAndWait() = 0;
    
    virtual void CheckAndResizeSwapChain() = 0;
    virtual void NewFrame() = 0;
    virtual void RenderFrame(ImDrawData& DrawData) = 0;
    virtual void PresentFrame() = 0;

public:
    void PostWindowCreated();
    void RequestExit();

    void WaitEvents() const;
    void PollEvents() const;
    void PostEmptyEvent() const;

    bool IsAppRunning() const;
    bool IsAppWindowOpen() const;
    bool IsAppWindowMinimized() const;

    void CloseAppWindow() const;
    void ShowAppWindow() const;
    void AddOnWindowCloseCallbackFunc(const std::function<void(uint32_t, void*)>& Func, void* UserData);
    void AddOnWindowRestoreCallbackFunc(const std::function<void(uint32_t WindowID, void* UserData)>& Func, void* UserData);

public:
    GLFWwindow* GetGLFWwindow() const { return m_AppWindow; }
    std::string GetIELogoPathString() const;
    std::string GetIEIconPathString() const;
    void DrawTelemetry() const;

private:
    void BroadcastOnWindowClosed() const;
    void BroadcastOnWindowRestored() const;

protected:
    GLFWwindow* m_AppWindow = nullptr;
    int32_t m_DefaultAppWindowWidth = 1280;
    int32_t m_DefaultAppWindowHeight = 720;

private:
    std::vector<std::pair<void*, std::function<void(uint32_t, void*)>>> m_OnWindowCloseCallbackFunc;
    std::vector<std::pair<void*, std::function<void(uint32_t, void*)>>> m_OnWindowRestoreCallbackFunc;

private:
    bool m_ExitRequested = false;
};

class IERenderer_Vulkan : public IERenderer
{
public:
    /* Begin IERenderer Implementation */
    IEResult Initialize() override;
    IEResult PostImGuiContextCreated() override;
    void Deinitialize() override;
    int32_t FlushGPUCommandsAndWait() override;
    
    void CheckAndResizeSwapChain() override;
    void NewFrame() override;
    void RenderFrame(ImDrawData& DrawData) override;
    void PresentFrame() override;
    /* End IERenderer Implementation */

private:
    static void GlfwErrorCallbackFunc(int ErrorCode, const char* Description);
    static void CheckVkResultFunc(VkResult err);

private:
    IEResult InitializeVulkan(const std::vector<const char*>& RequiredInstanceExtensionNames);
    IEResult InitializeInstancePhysicalDevice();
    void DinitializeVulkan();

private:
    ImGui_ImplVulkanH_Window m_AppWindowVulkanData = {};

    VkAllocationCallbacks* m_VkAllocationCallback = nullptr;
    VkInstance m_VkInstance = nullptr;
    VkPhysicalDevice m_VkPhysicalDevice = nullptr;
    VkDevice m_VkDevice = nullptr;
    VkQueue m_VkQueue = nullptr;
    VkDebugReportCallbackEXT m_VkDebugReportCallbackEXT = nullptr;
    VkPipelineCache m_VkPipelineCache = nullptr;
    VkDescriptorPool m_VkDescriptorPool = nullptr;

    uint32_t m_QueueFamilyIndex = static_cast<uint32_t>(-1);
    int m_MinImageCount = 2;
    bool m_SwapChainRebuild = false;
};