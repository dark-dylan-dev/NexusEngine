// SPDX-License-Identifier: MIT

export module NE.Engine.RHI.VulkanDevice;

import NE.Engine.RHI.Device;

import vk_mem_alloc;

export namespace Nexus::RHI {
    class VulkanDevice : public RendererDevice {
    public:
    private:
        void createInstance();

    private:
        vk::raii::Context m_Context;
        vk::raii::Instance m_Instance = nullptr;
        vk::DebugUtilsMessengerEXT m_DebugMessenger;
    };
} // namespace Nexus::RHI
