// SPDX-License-Identifier: MIT

module;

#include <Platform/DLL/Export.hpp>

export module NE.Engine.RHI.Vulkan.Surface;

import NE.Engine.RHI.Context;
import NE.Engine.RHI.Surface;

import NE.Engine.RHI.Vulkan.Device;
import NE.Engine.RHI.Vulkan.Texture;
import NE.Engine.RHI.Vulkan.TextureView;

import NE.Engine.Core.Types;
import NE.Engine.Core.Window;

import vulkan;

import std;

export namespace Nexus::RHI {
    class NEXUS_API VulkanSurface final : public Surface {
    public:
        VulkanSurface(VulkanDevice& device, const Window& window);
        ~VulkanSurface() override;

        VulkanSurface(const VulkanSurface&) = delete;
        VulkanSurface& operator=(const VulkanSurface&) = delete;

        VulkanSurface(VulkanSurface&&) = default;
        VulkanSurface& operator=(VulkanSurface&&) = default;

        RenderContext* BeginFrame() override;
        void ClearScreen(float r, float g, float b) override;
        void EndFrame() override;

        void Resize(int32 w, int32 h) override;

    private:
        vk::SurfaceKHR m_Surface;
        vk::SwapchainKHR m_Swapchain;

        VulkanDevice* m_ParentDevice = nullptr;

        std::vector<std::unique_ptr<VulkanTexture>> m_Textures;
        std::vector<std::unique_ptr<VulkanTextureView>> m_TextureViews;
        std::vector<vk::Semaphore> m_ImageAvailableSemaphores;
        std::vector<vk::Semaphore> m_RenderFinishedSemaphores;
        std::vector<vk::Fence> m_InFlightFences;
        std::vector<std::unique_ptr<RenderContext>> m_Contexts;
        uint32 m_CurrentFrame = 0;
        uint32 m_ImageIndex = 0;

        int m_Width = 0;
        int m_Height = 0;
    };
} // namespace Nexus::RHI
