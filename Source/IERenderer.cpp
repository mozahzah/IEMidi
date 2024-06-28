// Copyright © 2024 mozahzah (Incus Entertainment). All rights reserved.

#include "IERenderer.h"

#include "IEUtils.h"

void IERenderer::DrawTelemetry() const
{
    ImGuiIO& IO = ImGui::GetIO();

    uint32_t TelemetryWindowFlags = ImGuiWindowFlags_NoTitleBar |
                                    ImGuiWindowFlags_NoResize |
                                    ImGuiWindowFlags_NoMove |
                                    ImGuiWindowFlags_NoScrollbar |
                                    ImGuiWindowFlags_NoScrollWithMouse |
                                    ImGuiWindowFlags_NoCollapse |
                                    ImGuiWindowFlags_NoMouseInputs;
                                    
    ImGui::SetNextWindowPos(ImVec2(0.0f, IO.DisplaySize.y - ImGui::GetFrameHeight()));
    ImGui::Begin("Telemetry", nullptr, TelemetryWindowFlags);
    ImGui::Text("Frame Duration (ms): %.2f | FPS: %.0f", 1000.0f / IO.Framerate, IO.Framerate);
    ImGui::End();
}

IEResult IERenderer_Vulkan::Initialize()
{
    IEResult Result(IEResult::Type::Fail, "Failed to initialize IERenderer");

    glfwSetErrorCallback(&IERenderer_Vulkan::GlfwErrorCallbackFunc);
    if (glfwInit() && glfwVulkanSupported())
    {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        m_AppWindow = glfwCreateWindow(m_DefaultAppWindowWidth, m_DefaultAppWindowHeight, "IEMidi", nullptr, nullptr);
        if (m_AppWindow)
        {
            uint32_t RequiredInstanceExtensionCount = 0;
            if (const char** GlfwExtensions = glfwGetRequiredInstanceExtensions(&RequiredInstanceExtensionCount))
            {
                std::vector<const char*> RequiredInstanceExtensionNames(RequiredInstanceExtensionCount);
                for (int i = 0; i < RequiredInstanceExtensionCount; i++)
                {
                    RequiredInstanceExtensionNames[i] = GlfwExtensions[i];
                }

                if (InitializeVulkan(RequiredInstanceExtensionNames))
                {
                    if (glfwCreateWindowSurface(m_VkInstance, m_AppWindow, m_VkAllocationCallback, &m_AppWindowVulkanData.Surface) == VkResult::VK_SUCCESS)
                    {
                        glfwGetFramebufferSize(m_AppWindow, &m_DefaultAppWindowWidth, &m_DefaultAppWindowHeight);

                        VkBool32 PhysicalDeviceSurfaceSupport = false;
                        vkGetPhysicalDeviceSurfaceSupportKHR(m_VkPhysicalDevice, m_QueueFamilyIndex, m_AppWindowVulkanData.Surface, &PhysicalDeviceSurfaceSupport);
                        if (PhysicalDeviceSurfaceSupport == VK_TRUE)
                        {
                            Result.Type = IEResult::Type::Success;
                            Result.Message = "Successfully initialized IERenderer";
                        }
                        else
                        {
                            Result.Type = IEResult::Type::NotSupported;
                            Result.Message = "Physical device is not supported";
                        }
                    }
                }
            }
        }
    }
    return Result;
}

IEResult IERenderer_Vulkan::PostImGuiContextCreated()
{
    IEResult Result(IEResult::Type::Fail, "Failed to initialize ImGuiContext with Vulkan");

    const VkFormat RequestSurfaceImageFormats[4] = {  VK_FORMAT_B8G8R8A8_UNORM,
                                                      VK_FORMAT_R8G8B8A8_UNORM,
                                                      VK_FORMAT_B8G8R8_UNORM,
                                                      VK_FORMAT_R8G8B8_UNORM };

    const VkColorSpaceKHR RequestSurfaceColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
    m_AppWindowVulkanData.SurfaceFormat = ImGui_ImplVulkanH_SelectSurfaceFormat(m_VkPhysicalDevice, m_AppWindowVulkanData.Surface,
        RequestSurfaceImageFormats, std::size(RequestSurfaceImageFormats), RequestSurfaceColorSpace);

    VkPresentModeKHR PresentModeKHR[1] = { VK_PRESENT_MODE_FIFO_KHR };
    m_AppWindowVulkanData.PresentMode = ImGui_ImplVulkanH_SelectPresentMode(m_VkPhysicalDevice, m_AppWindowVulkanData.Surface, PresentModeKHR, std::size(PresentModeKHR));

    ImGui_ImplVulkanH_CreateOrResizeWindow(m_VkInstance, m_VkPhysicalDevice, m_VkDevice, &m_AppWindowVulkanData, m_QueueFamilyIndex, m_VkAllocationCallback,
        m_DefaultAppWindowWidth, m_DefaultAppWindowHeight, m_MinImageCount);

    if (ImGui_ImplGlfw_InitForVulkan(m_AppWindow, true))
    {
        ImGui_ImplVulkan_InitInfo VulkanInitInfo = {};
        VulkanInitInfo.Instance = m_VkInstance;
        VulkanInitInfo.PhysicalDevice = m_VkPhysicalDevice;
        VulkanInitInfo.Device = m_VkDevice;
        VulkanInitInfo.QueueFamily = m_QueueFamilyIndex;
        VulkanInitInfo.Queue = m_VkQueue;
        VulkanInitInfo.PipelineCache = m_VkPipelineCache;
        VulkanInitInfo.DescriptorPool = m_VkDescriptorPool;
        VulkanInitInfo.RenderPass = m_AppWindowVulkanData.RenderPass;
        VulkanInitInfo.Subpass = 0;
        VulkanInitInfo.MinImageCount = m_MinImageCount;
        VulkanInitInfo.ImageCount = m_AppWindowVulkanData.ImageCount;
        VulkanInitInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
        VulkanInitInfo.Allocator = m_VkAllocationCallback;
        VulkanInitInfo.MinAllocationSize = 1024 * 1024; // TODO Magic Number
        VulkanInitInfo.UseDynamicRendering = false;
        VulkanInitInfo.CheckVkResultFn = &IERenderer_Vulkan::CheckVkResultFunc;
        if (ImGui_ImplVulkan_Init(&VulkanInitInfo))
        {
            Result.Type = IEResult::Type::Success;
            Result.Message = "Successfully initialized ImGuiContext with Vulkan";
        }
    }
    return Result;
}

void IERenderer_Vulkan::Deinitialize()
{
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();

    ImGui::DestroyContext();
    ImGui_ImplVulkanH_DestroyWindow(m_VkInstance, m_VkDevice, &m_AppWindowVulkanData, m_VkAllocationCallback);

    DinitializeVulkan();

    glfwDestroyWindow(m_AppWindow);
    glfwTerminate();
}

int32_t IERenderer_Vulkan::FlushGPUCommandsAndWait()
{
    return vkDeviceWaitIdle(m_VkDevice);
}

bool IERenderer_Vulkan::IsAppWindowOpen() const
{
    bool bIsAppWindowOpen = false;
    if (m_AppWindow)
    {
        bIsAppWindowOpen = !glfwWindowShouldClose(m_AppWindow);
    }
    return bIsAppWindowOpen;
}

bool IERenderer_Vulkan::IsAppWindowMinimized() const
{
    bool bIsAppWindowMinimized = false;
    if (m_AppWindow)
    {
        if (glfwGetWindowAttrib(m_AppWindow, GLFW_ICONIFIED))
        {
            bIsAppWindowMinimized = true;
        }
    }
    return bIsAppWindowMinimized;
}

void IERenderer_Vulkan::PollEvents() const
{
    glfwPollEvents();
}

void IERenderer_Vulkan::CheckAndResizeSwapChain()
{
    int FrameBufferWidth = 0, FrameBufferHeight = 0;
    glfwGetFramebufferSize(m_AppWindow, &FrameBufferWidth, &FrameBufferHeight);

    if (FrameBufferWidth > 0 && FrameBufferHeight > 0 &&
        (m_SwapChainRebuild ||
            m_AppWindowVulkanData.Width != FrameBufferWidth ||
            m_AppWindowVulkanData.Height != FrameBufferHeight))
    {
        ImGui_ImplVulkan_SetMinImageCount(m_MinImageCount);
        ImGui_ImplVulkanH_CreateOrResizeWindow(m_VkInstance, m_VkPhysicalDevice,
            m_VkDevice, &m_AppWindowVulkanData, m_QueueFamilyIndex, m_VkAllocationCallback,
            FrameBufferWidth, FrameBufferHeight, m_MinImageCount);

        m_AppWindowVulkanData.FrameIndex = 0;
        m_SwapChainRebuild = false;
    }
}

void IERenderer_Vulkan::NewFrame()
{
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
}

void IERenderer_Vulkan::RenderFrame(ImDrawData& DrawData)
{
    const bool bIsMinimized = (DrawData.DisplaySize.x <= 0.0f || DrawData.DisplaySize.y <= 0.0f);
    if (!bIsMinimized)
    {
        m_AppWindowVulkanData.ClearValue.color.float32[0] = 0;
        m_AppWindowVulkanData.ClearValue.color.float32[1] = 0;
        m_AppWindowVulkanData.ClearValue.color.float32[2] = 0;
        m_AppWindowVulkanData.ClearValue.color.float32[3] = 0;

        VkSemaphore ImageAcquiredSemaphore = m_AppWindowVulkanData.FrameSemaphores[m_AppWindowVulkanData.SemaphoreIndex].ImageAcquiredSemaphore;
        VkSemaphore RenderCompleteSemaphore = m_AppWindowVulkanData.FrameSemaphores[m_AppWindowVulkanData.SemaphoreIndex].RenderCompleteSemaphore;

        const VkResult Result = vkAcquireNextImageKHR(m_VkDevice, m_AppWindowVulkanData.Swapchain, UINT64_MAX, ImageAcquiredSemaphore, VK_NULL_HANDLE, &m_AppWindowVulkanData.FrameIndex);
        if (Result == VK_ERROR_OUT_OF_DATE_KHR || Result == VK_SUBOPTIMAL_KHR)
        {
            m_SwapChainRebuild = true;
            return;
        }

        ImGui_ImplVulkanH_Frame& VulkanFrame = m_AppWindowVulkanData.Frames[m_AppWindowVulkanData.FrameIndex];
        if (vkWaitForFences(m_VkDevice, 1, &VulkanFrame.Fence, VK_TRUE, UINT64_MAX) == VkResult::VK_SUCCESS) // TODO Magic Number
        {
            if (vkResetFences(m_VkDevice, 1, &VulkanFrame.Fence) == VkResult::VK_SUCCESS)
            {
                if (vkResetCommandPool(m_VkDevice, VulkanFrame.CommandPool, 0) == VkResult::VK_SUCCESS) // TODO Magic Number
                {
                    VkCommandBufferBeginInfo CommandBufferBeginInfo = {};
                    CommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
                    CommandBufferBeginInfo.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
                    if (vkBeginCommandBuffer(VulkanFrame.CommandBuffer, &CommandBufferBeginInfo) == VkResult::VK_SUCCESS)
                    {
                        VkRenderPassBeginInfo RenderPassBeginInfo = {};
                        RenderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
                        RenderPassBeginInfo.renderPass = m_AppWindowVulkanData.RenderPass;
                        RenderPassBeginInfo.framebuffer = VulkanFrame.Framebuffer;
                        RenderPassBeginInfo.renderArea.extent.width = m_AppWindowVulkanData.Width;
                        RenderPassBeginInfo.renderArea.extent.height = m_AppWindowVulkanData.Height;
                        RenderPassBeginInfo.pClearValues = &m_AppWindowVulkanData.ClearValue;
                        RenderPassBeginInfo.clearValueCount = 1; // TODO Magic Number

                        vkCmdBeginRenderPass(VulkanFrame.CommandBuffer, &RenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
                        ImGui_ImplVulkan_RenderDrawData(&DrawData, VulkanFrame.CommandBuffer);
                        vkCmdEndRenderPass(VulkanFrame.CommandBuffer);

                        VkPipelineStageFlags PipelineStageFlags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
                        VkSubmitInfo SubmitInfo = {};
                        SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
                        SubmitInfo.waitSemaphoreCount = 1;
                        SubmitInfo.pWaitSemaphores = &ImageAcquiredSemaphore;
                        SubmitInfo.pWaitDstStageMask = &PipelineStageFlags;
                        SubmitInfo.commandBufferCount = 1; // TODO Magic Number
                        SubmitInfo.pCommandBuffers = &VulkanFrame.CommandBuffer;
                        SubmitInfo.signalSemaphoreCount = 1; // TODO Magic Number
                        SubmitInfo.pSignalSemaphores = &RenderCompleteSemaphore;

                        if (vkEndCommandBuffer(VulkanFrame.CommandBuffer) == VkResult::VK_SUCCESS)
                        {
                            vkQueueSubmit(m_VkQueue, 1, &SubmitInfo, VulkanFrame.Fence);
                        }
                    }
                }
            }
        }
    }
}

void IERenderer_Vulkan::PresentFrame()
{
    if (!m_SwapChainRebuild)
    {
        const VkSemaphore RenderCompleteSemaphore = m_AppWindowVulkanData.FrameSemaphores[m_AppWindowVulkanData.SemaphoreIndex].RenderCompleteSemaphore;

        VkPresentInfoKHR PresentInfoKHR = {};
        PresentInfoKHR.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        PresentInfoKHR.waitSemaphoreCount = 1; // TODO Magic Number
        PresentInfoKHR.pWaitSemaphores = &RenderCompleteSemaphore;
        PresentInfoKHR.swapchainCount = 1; // TODO Magic Number
        PresentInfoKHR.pSwapchains = &m_AppWindowVulkanData.Swapchain;
        PresentInfoKHR.pImageIndices = &m_AppWindowVulkanData.FrameIndex;

        const VkResult Result = vkQueuePresentKHR(m_VkQueue, &PresentInfoKHR);
        if (Result == VK_ERROR_OUT_OF_DATE_KHR || Result == VK_SUBOPTIMAL_KHR)
        {
            m_SwapChainRebuild = true;
        }
        else
        {
            m_AppWindowVulkanData.SemaphoreIndex = (m_AppWindowVulkanData.SemaphoreIndex + 1) % m_AppWindowVulkanData.SemaphoreCount;
        }
    }
}

void IERenderer_Vulkan::CheckVkResultFunc(VkResult Result)
{
    if (Result != VkResult::VK_SUCCESS)
    {
        std::fprintf(stderr, "Vulkan Error: VkResult = %d\n", Result);
        if (Result < 0)
        {
            abort();
        }
    }
}

void IERenderer_Vulkan::GlfwErrorCallbackFunc(int ErrorCode, const char* Description)
{
    if (ErrorCode)
    {
        std::fprintf(stderr, "Glfw Error: ErrorCode = %d, Description: %s", ErrorCode, Description);
    }
}

IEResult IERenderer_Vulkan::InitializeVulkan(const std::vector<const char*>& RequiredInstanceExtensionNames)
{
    IEResult Result(IEResult::Type::Fail, "Failed to initialize Vulkan");

    uint32_t InstanceExtensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &InstanceExtensionCount, nullptr);
    std::vector<VkExtensionProperties> InstanceExtensionProperties(InstanceExtensionCount);
    if (vkEnumerateInstanceExtensionProperties(nullptr, &InstanceExtensionCount, InstanceExtensionProperties.data()) == VkResult::VK_SUCCESS)
    {
        std::vector<const char*> InstanceExtensionNames(InstanceExtensionCount);
        for (int i = 0; i < InstanceExtensionCount; i++)
        {
            InstanceExtensionNames[i] = InstanceExtensionProperties[i].extensionName;
        }

        VkInstanceCreateInfo InstanceCreateInfo = {};
        InstanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        InstanceCreateInfo.enabledExtensionCount = InstanceExtensionCount;
        InstanceCreateInfo.ppEnabledExtensionNames = InstanceExtensionNames.data();

        for (const char* InstanceExtensionName : InstanceExtensionNames)
        {
            if (std::strcmp(InstanceExtensionName, VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME) == 0)
            {
                InstanceCreateInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
                break;
            }
        }

        if (vkCreateInstance(&InstanceCreateInfo, m_VkAllocationCallback, &m_VkInstance) == VkResult::VK_SUCCESS)
        {
            if (InitializeInstancePhysicalDevice())
            {
                uint32_t QueueFamilyCount = 0;
                vkGetPhysicalDeviceQueueFamilyProperties(m_VkPhysicalDevice, &QueueFamilyCount, nullptr);
                std::vector<VkQueueFamilyProperties> QueueFamilyProperties(QueueFamilyCount);
                vkGetPhysicalDeviceQueueFamilyProperties(m_VkPhysicalDevice, &QueueFamilyCount, QueueFamilyProperties.data());

                m_QueueFamilyIndex = static_cast<uint32_t>(-1);
                for (uint32_t i = 0; i < QueueFamilyCount; i++)
                {
                    if (QueueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
                    {
                        m_QueueFamilyIndex = i;
                        break;
                    }
                }

                if (m_QueueFamilyIndex != -1)
                {
                    uint32_t DeviceExtensionCount = 0;
                    vkEnumerateDeviceExtensionProperties(m_VkPhysicalDevice, nullptr, &DeviceExtensionCount, nullptr);
                    std::vector<VkExtensionProperties> DeviceExtensionProperties(DeviceExtensionCount);
                    vkEnumerateDeviceExtensionProperties(m_VkPhysicalDevice, nullptr, &DeviceExtensionCount, DeviceExtensionProperties.data());

                    std::vector<const char*> DeviceExtensionNames(DeviceExtensionCount);
                    for (int i = 0; i < DeviceExtensionCount; i++)
                    {
                        DeviceExtensionNames[i] = DeviceExtensionProperties[i].extensionName;
                    }

                    VkDeviceQueueCreateInfo DeviceQueueCreateInfo = {};
                    DeviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
                    DeviceQueueCreateInfo.queueFamilyIndex = m_QueueFamilyIndex;
                    DeviceQueueCreateInfo.queueCount = 1; // TODO Magic Number
                    float QueuePriority = 1.0f; // TODO Magic Number
                    DeviceQueueCreateInfo.pQueuePriorities = &QueuePriority;

                    VkDeviceCreateInfo DeviceCreateInfo = {};
                    DeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
                    DeviceCreateInfo.queueCreateInfoCount = 1; // TODO Magic Number
                    DeviceCreateInfo.pQueueCreateInfos = &DeviceQueueCreateInfo;
                    DeviceCreateInfo.enabledExtensionCount = (uint32_t)DeviceExtensionCount;
                    DeviceCreateInfo.ppEnabledExtensionNames = DeviceExtensionNames.data();

                    if (vkCreateDevice(m_VkPhysicalDevice, &DeviceCreateInfo, m_VkAllocationCallback, &m_VkDevice) == VkResult::VK_SUCCESS)
                    {
                        vkGetDeviceQueue(m_VkDevice, m_QueueFamilyIndex, 0, &m_VkQueue);

                        VkDescriptorPoolCreateInfo DescriptorPoolCreateInfo = {};
                        DescriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
                        DescriptorPoolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
                        DescriptorPoolCreateInfo.maxSets = 1; // TODO Magic Number
                        DescriptorPoolCreateInfo.poolSizeCount = 1; // TODO Magic Number

                        VkDescriptorPoolSize DescriptorPoolSize = {};
                        DescriptorPoolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                        DescriptorPoolSize.descriptorCount = 1; // TODO Magic Number

                        DescriptorPoolCreateInfo.pPoolSizes = &DescriptorPoolSize;

                        if (vkCreateDescriptorPool(m_VkDevice, &DescriptorPoolCreateInfo, m_VkAllocationCallback, &m_VkDescriptorPool) == VkResult::VK_SUCCESS)
                        {
                            Result.Type = IEResult::Type::Success;
                            Result.Message = "Successfully initialized Vulkan";
                        }
                    }
                }
            }
        }
    }
    return Result;
}

IEResult IERenderer_Vulkan::InitializeInstancePhysicalDevice()
{
    IEResult Result(IEResult::Type::Fail, "Failed to initialize instance physical device");

    uint32_t PhysicalDeviceCount = 0;
    vkEnumeratePhysicalDevices(m_VkInstance, &PhysicalDeviceCount, nullptr);
    std::vector<VkPhysicalDevice> PhysicalDevices(PhysicalDeviceCount);
    if (vkEnumeratePhysicalDevices(m_VkInstance, &PhysicalDeviceCount, PhysicalDevices.data()) == VkResult::VK_SUCCESS)
    {
        m_VkPhysicalDevice = PhysicalDevices[0];
        Result.Type = IEResult::Type::Success;
        Result.Message = "Using integrated device";

        for (VkPhysicalDevice& PhysicalDevice : PhysicalDevices)
        {
            VkPhysicalDeviceProperties PhysicalDeviceProperties;
            vkGetPhysicalDeviceProperties(PhysicalDevice, &PhysicalDeviceProperties);
            IELOG_INFO("Found %s", PhysicalDeviceProperties.deviceName);

            if (PhysicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            {
                m_VkPhysicalDevice = PhysicalDevice;
                Result.Type = IEResult::Type::Success;
                std::ostringstream ResultMessageStream;
                ResultMessageStream << "Using physical device " << PhysicalDeviceProperties.deviceName;
                Result.Message = ResultMessageStream.str();
                break;
            }
        }
    }
    return Result;
}

void IERenderer_Vulkan::DinitializeVulkan()
{
    vkDestroyDescriptorPool(m_VkDevice, m_VkDescriptorPool, m_VkAllocationCallback);
    vkDestroyDevice(m_VkDevice, m_VkAllocationCallback);
    vkDestroyInstance(m_VkInstance, m_VkAllocationCallback);
}