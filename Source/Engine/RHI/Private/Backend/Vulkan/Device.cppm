// SPDX-License-Identifier: MIT

module;

#include <Platform/DLL/Export.hpp>

export module NE.Engine.RHI.Vulkan.Device;

import NE.Engine.RHI.Device;
import NE.Engine.RHI.Surface;
import NE.Engine.RHI.TextureView;

import NE.Engine.Core.Types;
import NE.Engine.Core.Window;

import vk_mem_alloc;
import vulkan;

import std;

export namespace Nexus::RHI {
    class NEXUS_API VulkanDevice final : public Device {
    public:
        VulkanDevice();
        std::unique_ptr<Surface> CreateSurface(const Window& window) override;
        std::unique_ptr<TextureView> CreateTextureView(const TextureViewDesc& desc) override;

        vk::Device GetVulkanDevice() const {
            return m_Device;
        }
        vk::PhysicalDevice GetVulkanPhysicalDevice() const {
            return m_PhysicalDevice;
        }
        vk::Instance GetVulkanInstance() const {
            return m_Instance;
        }
        vk::Queue GetMainQueue() const {
            return m_MainQueue;
        }
        vk::CommandPool GetCommandPool() const {
            return m_CommandPool;
        }

        void WaitIdle() const override {
            m_Device.waitIdle();
        }

    private:
        void CreateInstance();
        void CreatePhysicalDevice();
        void CreateDevice();
        void CreateCommandPool();
        void CreateAllocator();

    private:
        vk::raii::Context m_Context;
        vk::raii::PhysicalDevice m_PhysicalDevice = nullptr;
        vk::raii::Instance m_Instance = nullptr;
        vk::raii::Device m_Device = nullptr;
        vk::raii::Queue m_MainQueue = nullptr;
        vk::DebugUtilsMessengerEXT m_DebugMessenger;
        uint32 m_MainQueueFamilyIndex;

        vk::raii::CommandPool m_CommandPool = nullptr;
        vma::raii::Allocator m_Allocator = nullptr;

        bool m_SupportRaytracing = false;
        bool m_SupportMeshShaders = false;
        uint64 m_OptimalRowPitchAlignment = 0;
        uint64 m_BufferImageGranularity = 0;
    };
} // namespace Nexus::RHI
