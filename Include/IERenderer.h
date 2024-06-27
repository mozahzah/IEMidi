// Copyright © 2024 mozahzah (Incus Entertainment). All rights reserved.

#pragma once

#define GLFW_INCLUDE_VULKAN
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

#include "GLFW/glfw3.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

#include "IECore.h"

class IERenderer
{
public:
    virtual ~IERenderer() = default;

public:
    virtual IEResult Initialize() { return IEResult(IEResult::Type::Unimplemented, "Unimplemented."); }
    virtual IEResult PostImGuiContextCreated() { return IEResult(IEResult::Type::Unimplemented, "Unimplemented."); }
    virtual void Deinitialize() {}
    virtual int32_t FlushGPUCommandsAndWait() { return -1; }
    
    virtual bool IsAppWindowOpen() const { return false; }
    virtual bool IsAppWindowMinimized() const { return true; }
    virtual void PollEvents() const {}
    virtual void CheckAndResizeSwapChain() {}

    virtual void NewFrame() {}
    virtual void RenderFrame(ImDrawData& DrawData) {}
    virtual void PresentFrame() {}

public:
    virtual void DrawTelemetry() const;
};

class IERenderer_Vulkan : public IERenderer
{
public:
    /* Begin IERenderer Implementation */
    IEResult Initialize() override;
    IEResult PostImGuiContextCreated() override;
    void Deinitialize() override;
    int32_t FlushGPUCommandsAndWait() override;
    
    bool IsAppWindowOpen() const override;
    bool IsAppWindowMinimized() const override;
    void PollEvents() const override;
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
    GLFWwindow* m_AppWindow = nullptr;
    ImGui_ImplVulkanH_Window m_AppWindowVulkanData = {};
    int32_t m_DefaultAppWindowWidth = 1280;
    int32_t m_DefaultAppWindowHeight = 720;

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