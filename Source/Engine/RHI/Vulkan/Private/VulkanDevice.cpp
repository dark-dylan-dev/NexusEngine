module;

#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_hpp_macros.hpp>

module NE.Engine.RHI.VulkanDevice;

import NE.Engine.Core.Config;
import NE.Engine.Core.Types;

import vk_mem_alloc;

static VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                          vk::DebugUtilsMessageTypeFlagsEXT messageType,
                                                          const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                          void* pUserData) {
    (void)messageType;
    (void)pUserData;

    switch (messageSeverity) {
        case vk::DebugUtilsMessageSeverityFlagBitsEXT::eError: {
            std::println("Vulkan Error: {}", pCallbackData->pMessage);
            break;
        }
        default:
            break;
    }
    return VK_FALSE;
}

namespace Nexus {

    void VulkanDevice::createInstance() {
        VULKAN_HPP_DEFAULT_DISPATCHER.init();

        vk::ApplicationInfo appInfo;
        appInfo.setApplicationVersion(vk::makeApiVersion(0, 0, 0, 1));
        appInfo.setEngineVersion(vk::makeApiVersion(0, 0, 0, 1));
        appInfo.setApiVersion(vk::ApiVersion14);
        appInfo.setPApplicationName("Nexus Engine");
        appInfo.setPEngineName("Nexus Engine");

        std::vector<const char*> instanceExtensions = {VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
                                                       VK_KHR_SURFACE_EXTENSION_NAME};
        if (glfwVulkanSupported()) {
            uint32_t glfwExtensionCount = 0;
            const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
            for (uint32 i = 0; i < glfwExtensionCount; i++) {
                instanceExtensions.push_back(glfwExtensions[i]);
            }
        }

        vk::InstanceCreateInfo instanceCreateInfo;
        instanceCreateInfo.setPApplicationInfo(&appInfo);
        instanceCreateInfo.setEnabledExtensionCount(static_cast<uint32_t>(instanceExtensions.size()));
        instanceCreateInfo.setPpEnabledExtensionNames(instanceExtensions.data());

        if constexpr(isDebug) {
            std::vector<const char*> instanceLayers = {"VK_LAYER_KHRONOS_validation"};
            instanceCreateInfo.setEnabledLayerCount(static_cast<uint32_t>(instanceLayers.size()));
            instanceCreateInfo.setPpEnabledLayerNames(instanceLayers.data());
        }

        m_Instance = vk::raii::Instance(m_Context, instanceCreateInfo);

        if constexpr (isDebug) {
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
        VULKAN_HPP_DEFAULT_DISPATCHER.init(static_cast<vk::Instance>(m_Instance));
    }
} // namespace Nexus