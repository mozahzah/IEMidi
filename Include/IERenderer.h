// Copyright © 2024 mozahzah (Incus Entertainment). All rights reserved.

#pragma once

#include <filesystem>
#include <memory>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include "glfw/glfw3.h"

#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

//#define APP_USE_UNLIMITED_FRAME_RATE
#ifdef _DEBUG
#define APP_USE_VULKAN_DEBUG_REPORT
#endif

#include "IECore.h"

class IERenderer
{
public:
    virtual ~IERenderer() = default;

public:
    virtual IEResult Initialize() { return IEResult(IEResult::Type::Unimplemented, "Unimplemented.", __func__); };
    virtual IEResult PostImGuiContextCreated() { return IEResult(IEResult::Type::Unimplemented, "Unimplemented.", __func__); };
    virtual void Deinitialize() {};
    virtual int32_t FlushGPUCommandsAndWait() { return -1; };

    virtual bool IsAppWindowOpen() {return false;};
    virtual void PollEvents() {};
    virtual void CheckAndResizeSwapChain() {};

    virtual void NewFrame() {};
    virtual void RenderFrame(ImDrawData& DrawData) {};
    virtual void PresentFrame() {};
};

class IERenderer_Vulkan : public IERenderer
{
public:
    /* Begin IERenderer Implementation */
    IEResult Initialize() override;
    IEResult PostImGuiContextCreated() override;
    void Deinitialize() override;
    int32_t FlushGPUCommandsAndWait() override;

    bool IsAppWindowOpen() override;
    void PollEvents() override;
    void CheckAndResizeSwapChain() override;

    void NewFrame() override;
    void RenderFrame(ImDrawData& DrawData) override;
    void PresentFrame() override;
    /* End IERenderer Implementation */

private:
    static void GlfwErrorCallbackFunc(int ErrorCode, const char* Description);
    static void CheckVkResultFunc(VkResult err);

private:
    bool InitializeVulkan(const std::vector<const char*>& RequiredInstanceExtensionNames);
    bool InitializeInstancePhysicalDevice();
    bool InitializeImGuiForVulkan();
    void DinitializeVulkan();

private:
    GLFWwindow* m_AppWindow = nullptr;
    ImGui_ImplVulkanH_Window m_AppWindowVulkanData;

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

    int32_t m_DefaultAppWindowWidth = 1280;
    int32_t m_DefaultAppWindowHeight = 720;
};