// SPDX-License-Identifier: MIT

module;

#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_hpp_macros.hpp>

module NE.Engine.RHI.Vulkan.Device;

import NE.Engine.RHI.Vulkan.Surface;
import NE.Engine.RHI.Vulkan.TextureView;

import NE.Engine.Core.Config;
import NE.Engine.Core.Types;

import vk_mem_alloc;

import std;

static VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                          vk::DebugUtilsMessageTypeFlagsEXT messageType,
                                                          const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                          void* pUserData) {
    static_cast<void>(messageType);
    static_cast<void>(pUserData);

    switch (messageSeverity) {
        case vk::DebugUtilsMessageSeverityFlagBitsEXT::eError: {
            std::println("Vulkan Error: {}", pCallbackData->pMessage);
            break;
        }
        default:
            break;
    }
    return vk::False;
}

namespace Nexus::RHI {

    VulkanDevice::VulkanDevice() {
        CreateInstance();
        CreatePhysicalDevice();
        CreateDevice();
        CreateCommandPool();
        CreateAllocator();
    }

    void VulkanDevice::CreateInstance() {
        VULKAN_HPP_DEFAULT_DISPATCHER.init();

        vk::ApplicationInfo appInfo;
        appInfo.setApplicationVersion(vk::makeApiVersion(0, 0, 1, 0));
        appInfo.setEngineVersion(vk::makeApiVersion(0, 0, 1, 0));
        appInfo.setApiVersion(vk::ApiVersion14);
        appInfo.setPApplicationName("Nexus Engine");
        appInfo.setPEngineName("Nexus Engine");

        std::vector<const char*> instanceExtensions = {vk::EXTDebugUtilsExtensionName,
                                                       vk::KHRSurfaceExtensionName};
        if (glfwVulkanSupported()) {
            uint32 glfwExtensionCount = 0;
            const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
            for (uint32 i = 0; i < glfwExtensionCount; i++) {
                instanceExtensions.push_back(glfwExtensions[i]);
            }
        }

        vk::InstanceCreateInfo instanceCreateInfo;
        instanceCreateInfo.setPApplicationInfo(&appInfo);
        instanceCreateInfo.setEnabledExtensionCount(static_cast<uint32>(instanceExtensions.size()));
        instanceCreateInfo.setPpEnabledExtensionNames(instanceExtensions.data());

        std::vector<const char*> instanceLayers;

        if constexpr (Config::isDebug) {
            // Query layers currently visible to the loader
            const auto availableLayers = m_Context.enumerateInstanceLayerProperties();
            bool validationLayerFound = false;

            for (const auto& layer : availableLayers) {
                if (std::strcmp(layer.layerName, "VK_LAYER_KHRONOS_validation") == 0) {
                    validationLayerFound = true;
                    break;
                }
            }

            if (validationLayerFound) {
                instanceLayers.push_back("VK_LAYER_KHRONOS_validation");
                instanceCreateInfo.setEnabledLayerCount(static_cast<uint32>(instanceLayers.size()));
                instanceCreateInfo.setPpEnabledLayerNames(instanceLayers.data());
            } else {
                // Log a warning or print to stderr that debugging validation is missing
                // but let the app continue running safely without crashing!
                std::println("Warning: VK_LAYER_KHRONOS_validation not found. Continuing without validation.");
            }
        }

        m_Instance = vk::raii::Instance(m_Context, instanceCreateInfo);

        if constexpr (Config::isDebug) {
            vk::DebugUtilsMessengerCreateInfoEXT messengerInfo = {};
            messengerInfo.messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eError |
                                            vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
                                            vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose;
            messengerInfo.messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
                                        vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
                                        vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance;
            messengerInfo.pfnUserCallback = VulkanDebugCallback;
            messengerInfo.pUserData = nullptr;
            vk::DebugUtilsMessengerCreateInfoEXT debugCreateInfo = messengerInfo;
            m_DebugMessenger = m_Instance.createDebugUtilsMessengerEXT(debugCreateInfo, nullptr);
        }

        // Initialize instance-specific function pointers
        VULKAN_HPP_DEFAULT_DISPATCHER.init(*m_Instance);
    }

    void VulkanDevice::CreatePhysicalDevice() {
        auto physicalDevices = m_Instance.enumeratePhysicalDevices();
        for (const auto& device : physicalDevices) {
            vk::PhysicalDeviceProperties properties = device.getProperties();
            if (properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu) {
                m_PhysicalDevice = device;
                break;
            }
        }

        auto properties = m_PhysicalDevice.getProperties();
        std::println("Using Vulkan device: {}", properties.deviceName.data());

        m_OptimalRowPitchAlignment = properties.limits.optimalBufferCopyRowPitchAlignment;
        m_BufferImageGranularity = properties.limits.bufferImageGranularity;
    }

    void VulkanDevice::CreateDevice() {
        std::vector<const char*> mandatoryExtensions = {vk::KHRSwapchainExtensionName,
                                                        vk::EXTDescriptorIndexingExtensionName,
                                                        vk::KHRDynamicRenderingExtensionName,
                                                        vk::EXTMutableDescriptorTypeExtensionName,
                                                        vk::KHRBufferDeviceAddressExtensionName,
                                                        vk::KHRSynchronization2ExtensionName};

        std::map<const char*, bool> optionalExtensions = {{vk::EXTMeshShaderExtensionName, false},
                                                          {vk::KHRRayQueryExtensionName, false},
                                                          {vk::KHRAccelerationStructureExtensionName, false},
                                                          {vk::KHRRayTracingPipelineExtensionName, false},
                                                          {vk::KHRDeferredHostOperationsExtensionName, false}};


        const auto availableExtensions = m_PhysicalDevice.enumerateDeviceExtensionProperties();
        for (const auto& availableExtension : availableExtensions) {
            for (auto& [extension, hasExtension] : optionalExtensions) {
                if (strcmp(availableExtension.extensionName, extension) == 0) {
                    hasExtension = true;
                    break;
                }
            }
        }

        for (const auto& [extension, hasExtension] : optionalExtensions) {
            if (hasExtension) {
                mandatoryExtensions.push_back(extension);
                std::println("Enabling optional Vulkan extension: {}", extension);
            }
        }

        vk::PhysicalDeviceVulkan11Features vulkan11Features;
        vulkan11Features.setShaderDrawParameters(true);

        vk::PhysicalDeviceVulkan12Features vulkan12Features;
        vulkan12Features.setBufferDeviceAddress(true);
        vulkan12Features.setDescriptorIndexing(true);
        vulkan12Features.setRuntimeDescriptorArray(true);
        vulkan12Features.setShaderSampledImageArrayNonUniformIndexing(true);
        vulkan12Features.setDescriptorBindingVariableDescriptorCount(true);
        vulkan12Features.setDescriptorBindingPartiallyBound(true);
        vulkan12Features.setDescriptorBindingSampledImageUpdateAfterBind(true);
        vulkan12Features.setPNext(&vulkan11Features);

        vk::PhysicalDeviceVulkan13Features vulkan13Features;
        vulkan13Features.setDynamicRendering(true);
        vulkan13Features.setSynchronization2(true);
        vulkan13Features.setPNext(&vulkan12Features);

        vk::PhysicalDeviceMutableDescriptorTypeFeaturesEXT mutableDescriptorTypeFeatures;
        mutableDescriptorTypeFeatures.setMutableDescriptorType(true);
        mutableDescriptorTypeFeatures.setPNext(&vulkan13Features);

        void* lastPNext = &mutableDescriptorTypeFeatures;

        vk::PhysicalDeviceMeshShaderFeaturesEXT meshShaderFeatures;
        vk::PhysicalDeviceRayQueryFeaturesKHR rayQueryFeatures;
        vk::PhysicalDeviceAccelerationStructureFeaturesKHR accelerationStructureFeatures;
        vk::PhysicalDeviceRayTracingPipelineFeaturesKHR rayTracingPipelineFeatures;

        if (optionalExtensions[vk::EXTMeshShaderExtensionName]) {
            meshShaderFeatures.setMeshShader(true);
            meshShaderFeatures.setTaskShader(true);
            meshShaderFeatures.setPNext(lastPNext);
            lastPNext = &meshShaderFeatures;

            m_SupportMeshShaders = true;
        }


        if (optionalExtensions[vk::KHRAccelerationStructureExtensionName]) {
            accelerationStructureFeatures.setAccelerationStructure(true);
            accelerationStructureFeatures.setDescriptorBindingAccelerationStructureUpdateAfterBind(true);
            accelerationStructureFeatures.setPNext(lastPNext);
            lastPNext = &accelerationStructureFeatures;
        }

        if (optionalExtensions[vk::KHRRayTracingPipelineExtensionName]) {
            rayTracingPipelineFeatures.setRayTracingPipeline(true);
            rayTracingPipelineFeatures.setRayTracingPipelineTraceRaysIndirect(true);
            rayTracingPipelineFeatures.setRayTraversalPrimitiveCulling(true);
            rayTracingPipelineFeatures.setPNext(lastPNext);
            lastPNext = &rayTracingPipelineFeatures;
        }

        const bool supportsRayQuery =
            optionalExtensions[vk::KHRRayQueryExtensionName] &&
            optionalExtensions[vk::KHRAccelerationStructureExtensionName];

        if (supportsRayQuery) {
            rayQueryFeatures.setRayQuery(true);
            rayQueryFeatures.setPNext(lastPNext);
            lastPNext = &rayQueryFeatures;

            // Raytracing only supported if both ray query and acceleration structure features are available
            m_SupportRaytracing = true;
        }

        vk::PhysicalDeviceFeatures2 deviceFeatures;
        deviceFeatures.features.setSamplerAnisotropy(true);
        deviceFeatures.features.setDepthClamp(true);
        deviceFeatures.features.setFillModeNonSolid(true);
        deviceFeatures.features.setMultiDrawIndirect(true);
        deviceFeatures.features.setTextureCompressionBC(true);
        deviceFeatures.features.setDrawIndirectFirstInstance(true);
        deviceFeatures.setPNext(lastPNext);

        // Queues
        auto queues = m_PhysicalDevice.getQueueFamilyProperties();
        for (uint32 i = 0; i < static_cast<uint32>(queues.size()); i++) {
            if (queues[i].queueFlags & vk::QueueFlagBits::eGraphics) {
                m_MainQueueFamilyIndex = i;
                break;
            }
        }

        float queuePriority = 1.0f;
        vk::DeviceQueueCreateInfo queueCreateInfo;
        queueCreateInfo.setQueueFamilyIndex(m_MainQueueFamilyIndex);
        queueCreateInfo.setQueueCount(1);
        queueCreateInfo.setPQueuePriorities(&queuePriority);

        vk::DeviceCreateInfo deviceCreateInfo;
        deviceCreateInfo.setPNext(&deviceFeatures);
        deviceCreateInfo.setQueueCreateInfoCount(1);
        deviceCreateInfo.setPQueueCreateInfos(&queueCreateInfo);
        deviceCreateInfo.setEnabledExtensionCount(static_cast<uint32>(mandatoryExtensions.size()));
        deviceCreateInfo.setPpEnabledExtensionNames(mandatoryExtensions.data());

        m_Device = m_PhysicalDevice.createDevice(deviceCreateInfo, nullptr);
        m_MainQueue = m_Device.getQueue(m_MainQueueFamilyIndex, 0);

        VULKAN_HPP_DEFAULT_DISPATCHER.init(static_cast<vk::Device>(m_Device));
    }

    void VulkanDevice::CreateCommandPool() {
        vk::CommandPoolCreateInfo poolCreateInfo;
        poolCreateInfo.setQueueFamilyIndex(m_MainQueueFamilyIndex);
        poolCreateInfo.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);

        m_CommandPool = m_Device.createCommandPool(poolCreateInfo);
    }

    void VulkanDevice::CreateAllocator() {
        vma::VulkanFunctions vulkanFunctions = {};
        vulkanFunctions.vkGetInstanceProcAddr = VULKAN_HPP_DEFAULT_DISPATCHER.vkGetInstanceProcAddr;
        vulkanFunctions.vkGetDeviceProcAddr = VULKAN_HPP_DEFAULT_DISPATCHER.vkGetDeviceProcAddr;

        vma::AllocatorCreateInfo allocatorInfo = {};
        allocatorInfo.physicalDevice = m_PhysicalDevice;
        allocatorInfo.vulkanApiVersion = vk::ApiVersion14;
        allocatorInfo.flags = vma::AllocatorCreateFlagBits::eBufferDeviceAddress;

        m_Allocator = vma::raii::createAllocator(m_Instance, m_Device, allocatorInfo);
    }

    std::unique_ptr<Surface> VulkanDevice::CreateSurface(const Window& window) {
        return std::make_unique<VulkanSurface>(*this, window);
    }

    std::unique_ptr<TextureView> VulkanDevice::CreateTextureView(const TextureViewDesc& desc) {
        return std::make_unique<VulkanTextureView>(this, desc);
    }
} // namespace Nexus::RHI
