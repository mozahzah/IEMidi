// Copyright © 2024 mozahzah (Incus Entertainment). All rights reserved.

#include "IERenderer.h"

bool IERenderer_Vulkan::Initialize()
{
    glfwSetErrorCallback([](int,const char* m)
    {
        printf("%s", m);
    });
    if (!glfwInit())
        return 1;

    // Create window with Vulkan context
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* Window = glfwCreateWindow(1280, 720, "Dear ImGui GLFW+Vulkan example", nullptr, nullptr);
    if (!glfwVulkanSupported())
    {
        printf("GLFW: Vulkan Not Supported\n");
        return 1;
    }

    std::vector<const char*> Extensions;
    uint32_t extensions_count = 0;
    const char** glfw_extensions = glfwGetRequiredInstanceExtensions(&extensions_count);
    for (uint32_t i = 0; i < extensions_count; i++)
    {
        Extensions.push_back(glfw_extensions[i]);
    }
    
    InitializeVulkan(Extensions);

    // Create Window Surface
    VkSurfaceKHR Surface;
    VkResult err = glfwCreateWindowSurface(m_VkInstance, Window, m_VkAllocationCallback, &Surface);
    //check_vk_result(err);

    // Create Framebuffers
    int w, h;
    glfwGetFramebufferSize(Window, &w, &h);
    ImGui_ImplVulkanH_Window* wd = &m_AppWindowVulkanData;
    InitializeVulkanWindow(wd, Surface, w, h);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForVulkan(Window, true);
    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = m_VkInstance;
    init_info.PhysicalDevice = m_VkPhysicalDevice;
    init_info.Device = m_VkDevice;
    init_info.QueueFamily = m_QueueFamily;
    init_info.Queue = m_VkQueue;
    init_info.PipelineCache = m_VkPipelineCache;
    init_info.DescriptorPool = m_VkDescriptorPool;
    init_info.RenderPass =  m_AppWindowVulkanData.RenderPass;
    init_info.Subpass = 0;
    init_info.MinImageCount = m_MinImageCount;
    init_info.ImageCount =  m_AppWindowVulkanData.ImageCount;
    init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    init_info.Allocator = m_VkAllocationCallback;
    //init_info.CheckVkResultFn = check_vk_result;
    ImGui_ImplVulkan_Init(&init_info);

    return true;
}

void IERenderer_Vulkan::Deinitialize()
{
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
}

void IERenderer_Vulkan::Cleanup()
{
    ImGui_ImplVulkanH_DestroyWindow(m_VkInstance, m_VkDevice, &m_AppWindowVulkanData, m_VkAllocationCallback);

    vkDestroyDescriptorPool(m_VkDevice, m_VkDescriptorPool, m_VkAllocationCallback);
    vkDestroyDevice(m_VkDevice, m_VkAllocationCallback);
    vkDestroyInstance(m_VkInstance, m_VkAllocationCallback);

    glfwDestroyWindow(m_AppWindow);
    glfwTerminate();
}

int32_t IERenderer_Vulkan::FlushGPUCommandsAndWait()
{
    return vkDeviceWaitIdle(m_VkDevice);
}

bool IERenderer_Vulkan::IsAppWindowClosed()
{
    return false;
}

void IERenderer_Vulkan::PollEvents()
{
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
            m_VkDevice, &m_AppWindowVulkanData, m_QueueFamily, m_VkAllocationCallback,
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
        ImVec4 AppWindowColor;
        m_AppWindowVulkanData.ClearValue.color.float32[0] = AppWindowColor.x * AppWindowColor.w;
        m_AppWindowVulkanData.ClearValue.color.float32[1] = AppWindowColor.y * AppWindowColor.w;
        m_AppWindowVulkanData.ClearValue.color.float32[2] = AppWindowColor.z * AppWindowColor.w;
        m_AppWindowVulkanData.ClearValue.color.float32[3] = AppWindowColor.w;

        VkSemaphore image_acquired_semaphore  = m_AppWindowVulkanData.FrameSemaphores[m_AppWindowVulkanData.SemaphoreIndex].ImageAcquiredSemaphore;
        VkSemaphore render_complete_semaphore = m_AppWindowVulkanData.FrameSemaphores[m_AppWindowVulkanData.SemaphoreIndex].RenderCompleteSemaphore;
    
        VkResult Result = vkAcquireNextImageKHR(m_VkDevice, m_AppWindowVulkanData.Swapchain, UINT64_MAX, image_acquired_semaphore, VK_NULL_HANDLE, &m_AppWindowVulkanData.FrameIndex);
        if (Result == VK_ERROR_OUT_OF_DATE_KHR || Result == VK_SUBOPTIMAL_KHR)
        {
            m_SwapChainRebuild = true;
            return;
        }

        ImGui_ImplVulkanH_Frame& VulkanFrame = m_AppWindowVulkanData.Frames[m_AppWindowVulkanData.FrameIndex];
        
        Result = vkWaitForFences(m_VkDevice, 1, &VulkanFrame.Fence, VK_TRUE, UINT64_MAX);    // wait indefinitely instead of periodically checking
        //check_vk_result(err);
        Result = vkResetFences(m_VkDevice, 1, &VulkanFrame.Fence);
        //check_vk_result(err);
        Result = vkResetCommandPool(m_VkDevice, VulkanFrame.CommandPool, 0);
        //check_vk_result(err);

        VkCommandBufferBeginInfo CommandBufferBeginInfo = {};
        CommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        CommandBufferBeginInfo.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        Result = vkBeginCommandBuffer(VulkanFrame.CommandBuffer, &CommandBufferBeginInfo);
        // check err
    
        VkRenderPassBeginInfo RenderPassBeginInfo = {};
        RenderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        RenderPassBeginInfo.renderPass =   m_AppWindowVulkanData.RenderPass;
        RenderPassBeginInfo.framebuffer = VulkanFrame.Framebuffer;
        RenderPassBeginInfo.renderArea.extent.width =  m_AppWindowVulkanData.Width;
        RenderPassBeginInfo.renderArea.extent.height = m_AppWindowVulkanData.Height;
        RenderPassBeginInfo.clearValueCount = 1;
        RenderPassBeginInfo.pClearValues = &m_AppWindowVulkanData.ClearValue;
        vkCmdBeginRenderPass(VulkanFrame.CommandBuffer, &RenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
        
        // Record dear imgui primitives into command buffer
        ImGui_ImplVulkan_RenderDrawData(&DrawData, VulkanFrame.CommandBuffer);

        // Submit command buffer
        vkCmdEndRenderPass(VulkanFrame.CommandBuffer);
        
        VkPipelineStageFlags PipelineStageFlags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        VkSubmitInfo SubmitInfo = {};
        SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        SubmitInfo.waitSemaphoreCount = 1;
        SubmitInfo.pWaitSemaphores = &image_acquired_semaphore;
        SubmitInfo.pWaitDstStageMask = &PipelineStageFlags;
        SubmitInfo.commandBufferCount = 1;
        SubmitInfo.pCommandBuffers = &VulkanFrame.CommandBuffer;
        SubmitInfo.signalSemaphoreCount = 1;
        SubmitInfo.pSignalSemaphores = &render_complete_semaphore;

        Result = vkEndCommandBuffer(VulkanFrame.CommandBuffer);
        //check_vk_result(err);
        Result = vkQueueSubmit(m_VkQueue, 1, &SubmitInfo, VulkanFrame.Fence);
        //check_vk_result(err);
    }
}

void IERenderer_Vulkan::PresentFrame()
{
    if (!m_SwapChainRebuild)
    {
        const VkSemaphore RenderCompleteSemaphore = m_AppWindowVulkanData.FrameSemaphores[m_AppWindowVulkanData.SemaphoreIndex].RenderCompleteSemaphore;

        VkPresentInfoKHR PresentInfoKHR = {};
        PresentInfoKHR.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        PresentInfoKHR.waitSemaphoreCount = 1;
        PresentInfoKHR.pWaitSemaphores = &RenderCompleteSemaphore;
        PresentInfoKHR.swapchainCount = 1;
        PresentInfoKHR.pSwapchains = &m_AppWindowVulkanData.Swapchain;
        PresentInfoKHR.pImageIndices = &m_AppWindowVulkanData.FrameIndex;

        const VkResult Result = vkQueuePresentKHR(m_VkQueue, &PresentInfoKHR);
        if (Result == VK_ERROR_OUT_OF_DATE_KHR || Result == VK_SUBOPTIMAL_KHR)
        {
            m_SwapChainRebuild = true;
        }
        else
        {
            m_AppWindowVulkanData.SemaphoreIndex = m_AppWindowVulkanData.SemaphoreIndex + 1 % m_AppWindowVulkanData.SemaphoreCount;
        }
    }
}

VkPhysicalDevice IERenderer_Vulkan::SetupVulkan_SelectPhysicalDevice()
{
    uint32_t GPUCount;
    VkResult Result = vkEnumeratePhysicalDevices(m_VkInstance, &GPUCount, nullptr);
    //check_vk_result(err);
    IM_ASSERT(GPUCount > 0);

    std::vector<VkPhysicalDevice> GPUs;

    Result = vkEnumeratePhysicalDevices(m_VkInstance, &GPUCount, GPUs.data());
    //check_vk_result(err);

    for (VkPhysicalDevice& GPU : GPUs)
    {
        VkPhysicalDeviceProperties PhysicalDeviceProperties;
        vkGetPhysicalDeviceProperties(GPU, &PhysicalDeviceProperties);
        if (PhysicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        {
            return GPU;
        }
    }

    // Use first GPU (Integrated) is a Discrete one is not available.
    if (GPUCount > 0)
    {
        return GPUs[0];
    }   
    return nullptr;
}

bool IERenderer_Vulkan::IsExtensionAvailable(const std::vector<VkExtensionProperties>& ExtensionProperties, const char* ExtensionName)
{
    for (const VkExtensionProperties& ExtensionPropertie : ExtensionProperties)
    {
        if (strcmp(ExtensionPropertie.extensionName, ExtensionName) == 0)
        {
            return true;
        }
    }  
    return false;
}

void IERenderer_Vulkan::InitializeVulkan(std::vector<const char*>& InstanceExtensions)
{
        VkResult err;
#ifdef IMGUI_IMPL_VULKAN_USE_VOLK
    volkInitialize();
#endif

    // Create Vulkan Instance
    {
        VkInstanceCreateInfo create_info = {};
        create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

        // Enumerate available extensions
        uint32_t ExtensionPropertiesCount = 0;
        std::vector<VkExtensionProperties> ExtensionProperties;
        vkEnumerateInstanceExtensionProperties(nullptr, &ExtensionPropertiesCount, nullptr);
        ExtensionProperties.resize(ExtensionPropertiesCount);
        err = vkEnumerateInstanceExtensionProperties(nullptr, &ExtensionPropertiesCount, ExtensionProperties.data());
        //check_vk_result(err);

        // Enable required extensions
        if (IsExtensionAvailable(ExtensionProperties, VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME))
            InstanceExtensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
#ifdef VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME
        if (IsExtensionAvailable(ExtensionProperties, VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME))
        {
            InstanceExtensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
            create_info.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
        }
#endif
        // Create Vulkan Instance
        create_info.enabledExtensionCount = InstanceExtensions.size();
        create_info.ppEnabledExtensionNames = InstanceExtensions.data();
        err = vkCreateInstance(&create_info, m_VkAllocationCallback, &m_VkInstance);
        //check_vk_result(err);
#ifdef IMGUI_IMPL_VULKAN_USE_VOLK
        volkLoadInstance(g_Instance);
#endif
    }

    // Select Physical Device (GPU)
    m_VkPhysicalDevice = SetupVulkan_SelectPhysicalDevice();

    // Select graphics queue family
    {
        uint32_t count;
        vkGetPhysicalDeviceQueueFamilyProperties(m_VkPhysicalDevice, &count, nullptr);
        VkQueueFamilyProperties* queues = (VkQueueFamilyProperties*)malloc(sizeof(VkQueueFamilyProperties) * count);
        vkGetPhysicalDeviceQueueFamilyProperties(m_VkPhysicalDevice, &count, queues);
        for (uint32_t i = 0; i < count; i++)
            if (queues[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                m_QueueFamily = i;
                break;
            }
        free(queues);
        IM_ASSERT(m_QueueFamily != (uint32_t)-1);
    }

    // Create Logical Device (with 1 queue)
    {
        ImVector<const char*> device_extensions;
        device_extensions.push_back("VK_KHR_swapchain");

        // Enumerate physical device extension
        uint32_t properties_count;
        ImVector<VkExtensionProperties> properties;
        vkEnumerateDeviceExtensionProperties(m_VkPhysicalDevice, nullptr, &properties_count, nullptr);
        properties.resize(properties_count);
        vkEnumerateDeviceExtensionProperties(m_VkPhysicalDevice, nullptr, &properties_count, properties.Data);
#ifdef VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME
        if (IsExtensionAvailable(properties, VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME))
            device_extensions.push_back(VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME);
#endif

        const float queue_priority[] = { 1.0f };
        VkDeviceQueueCreateInfo queue_info[1] = {};
        queue_info[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_info[0].queueFamilyIndex = m_QueueFamily;
        queue_info[0].queueCount = 1;
        queue_info[0].pQueuePriorities = queue_priority;
        VkDeviceCreateInfo create_info = {};
        create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        create_info.queueCreateInfoCount = sizeof(queue_info) / sizeof(queue_info[0]);
        create_info.pQueueCreateInfos = queue_info;
        create_info.enabledExtensionCount = (uint32_t)device_extensions.Size;
        create_info.ppEnabledExtensionNames = device_extensions.Data;
        err = vkCreateDevice(m_VkPhysicalDevice, &create_info, m_VkAllocationCallback, &m_VkDevice);
        //check_vk_result(err);
        vkGetDeviceQueue(m_VkDevice, m_QueueFamily, 0, &m_VkQueue);
    }

    // Create Descriptor Pool
    // The example only requires a single combined image sampler descriptor for the font image and only uses one descriptor set (for that)
    // If you wish to load e.g. additional textures you may need to alter pools sizes.
    {
        VkDescriptorPoolSize pool_sizes[] =
        {
            { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1 },
        };
        VkDescriptorPoolCreateInfo pool_info = {};
        pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        pool_info.maxSets = 1;
        pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
        pool_info.pPoolSizes = pool_sizes;
        err = vkCreateDescriptorPool(m_VkDevice, &pool_info, m_VkAllocationCallback, &m_VkDescriptorPool);
        //check_vk_result(err);
    }
}

void IERenderer_Vulkan::InitializeVulkanWindow(ImGui_ImplVulkanH_Window* wd, VkSurfaceKHR surface, int width, int height)
{
    m_AppWindowVulkanData.Surface = surface;

    // Check for WSI support
    VkBool32 res;
    vkGetPhysicalDeviceSurfaceSupportKHR(m_VkPhysicalDevice, m_QueueFamily, m_AppWindowVulkanData.Surface, &res);
    if (res != VK_TRUE)
    {
        fprintf(stderr, "Error no WSI support on physical device 0\n");
        exit(-1);
    }

    // Select Surface Format
    const VkFormat requestSurfaceImageFormat[] = { VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_B8G8R8_UNORM, VK_FORMAT_R8G8B8_UNORM };
    const VkColorSpaceKHR requestSurfaceColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
    m_AppWindowVulkanData.SurfaceFormat = ImGui_ImplVulkanH_SelectSurfaceFormat(m_VkPhysicalDevice,   m_AppWindowVulkanData.Surface, requestSurfaceImageFormat, (size_t)IM_ARRAYSIZE(requestSurfaceImageFormat), requestSurfaceColorSpace);

    // Select Present Mode
#ifdef APP_USE_UNLIMITED_FRAME_RATE
    VkPresentModeKHR present_modes[] = { VK_PRESENT_MODE_MAILBOX_KHR, VK_PRESENT_MODE_IMMEDIATE_KHR, VK_PRESENT_MODE_FIFO_KHR };
#else
    VkPresentModeKHR present_modes[] = { VK_PRESENT_MODE_FIFO_KHR };
#endif
    m_AppWindowVulkanData.PresentMode = ImGui_ImplVulkanH_SelectPresentMode(m_VkPhysicalDevice,   m_AppWindowVulkanData.Surface, &present_modes[0], IM_ARRAYSIZE(present_modes));
    //printf("[vulkan] Selected PresentMode = %d\n",    m_AppWindowVulkanData.PresentMode);

    // Create SwapChain, RenderPass, Framebuffer, etc.
    IM_ASSERT(m_MinImageCount >= 2);
    ImGui_ImplVulkanH_CreateOrResizeWindow(m_VkInstance, m_VkPhysicalDevice, m_VkDevice, wd, m_QueueFamily, m_VkAllocationCallback, width, height, m_MinImageCount);
}

void IERenderer_Vulkan::DinitializeVulkan()
{
    vkDestroyDescriptorPool(m_VkDevice, m_VkDescriptorPool, m_VkAllocationCallback);
    vkDestroyDevice(m_VkDevice, m_VkAllocationCallback);
    vkDestroyInstance(m_VkInstance, m_VkAllocationCallback);
}