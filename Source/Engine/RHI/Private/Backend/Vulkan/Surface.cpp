// SPDX-License-Identifier: MIT

module;

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

module NE.Engine.RHI.Vulkan.Surface;

import NE.Engine.RHI.Texture;
import NE.Engine.RHI.TextureView;

import NE.Engine.RHI.Vulkan.Context;
import NE.Engine.RHI.Vulkan.Texture;

import NE.Engine.Core.Types;

import vulkan;

import std;

static vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const Nexus::RHI::VulkanDevice* device, const vk::SurfaceKHR& surface) {
    std::vector<vk::SurfaceFormatKHR> availableFormats = device->GetVulkanPhysicalDevice().getSurfaceFormatsKHR(surface);

    if (availableFormats.empty()) {
        std::unreachable();
    }

    const auto formatIt = std::ranges::find_if(availableFormats, [](const auto& format) {
        return format.format == vk::Format::eR8G8B8A8Srgb && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear;
    });

    const auto formatIt2 = std::ranges::find_if(availableFormats, [](const auto& format) {
        return format.format == vk::Format::eR8G8B8A8Unorm && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear;
    });

    if (formatIt != availableFormats.end())
        return *formatIt;
    if (formatIt2 != availableFormats.end())
        return *formatIt2;

    return availableFormats[0];
}

namespace Nexus::RHI {
    VulkanSurface::VulkanSurface(VulkanDevice& device, const Window& window) : m_ParentDevice(&device) {
        VkSurfaceKHR surface = VK_NULL_HANDLE;
        VkResult result = glfwCreateWindowSurface(m_ParentDevice->GetVulkanInstance(),
                                                  static_cast<GLFWwindow*>(window.GetHandle()), nullptr, &surface);
        if (result != VK_SUCCESS) {
            std::println("Failed to create surface!");
            std::unreachable();
        }

        m_Surface = vk::SurfaceKHR(surface);

        m_Width = window.GetWidth();
        m_Height = window.GetHeight();

        // Swapchain
        auto swapChainSurfaceFormat = chooseSwapSurfaceFormat(m_ParentDevice, m_Surface);
        const auto availablePresentModes =
            m_ParentDevice->GetVulkanPhysicalDevice().getSurfacePresentModesKHR(m_Surface);
        if (!std::ranges::any_of(availablePresentModes,
                                 [](auto presentMode) { return presentMode == vk::PresentModeKHR::eFifo; })) {
            std::unreachable();
        }

        vk::SwapchainCreateInfoKHR swapchainCreateInfo;
        swapchainCreateInfo.setSurface(m_Surface);
        swapchainCreateInfo.setMinImageCount(2);
        swapchainCreateInfo.setImageFormat(swapChainSurfaceFormat.format);
        swapchainCreateInfo.setImageColorSpace(swapChainSurfaceFormat.colorSpace);
        swapchainCreateInfo.setImageExtent(
            vk::Extent2D{static_cast<uint32>(m_Width), static_cast<uint32>(m_Height)});
        swapchainCreateInfo.setImageArrayLayers(1);
        swapchainCreateInfo.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment);
        swapchainCreateInfo.setImageSharingMode(vk::SharingMode::eExclusive);
        swapchainCreateInfo.setPreTransform(vk::SurfaceTransformFlagBitsKHR::eIdentity);
        swapchainCreateInfo.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque);
        swapchainCreateInfo.setPresentMode(vk::PresentModeKHR::eFifo);
        swapchainCreateInfo.setClipped(vk::True);

        m_Swapchain = m_ParentDevice->GetVulkanDevice().createSwapchainKHR(swapchainCreateInfo);

        auto swapchainImages = m_ParentDevice->GetVulkanDevice().getSwapchainImagesKHR(m_Swapchain);
        for (uint64 i = 0; i < swapchainImages.size(); i++) {
            TextureDesc desc;
            desc.Width = window.GetWidth();
            desc.Height = window.GetHeight();
            desc.Format = TextureFormat::RGBA8_SRGB;
            desc.Usage = TextureUsage::RENDER_TARGET;

            auto texture = std::make_unique<VulkanTexture>(VulkanTexture::AdoptExternal(swapchainImages[i], m_ParentDevice, desc,
                                                                          "Swapchain Texture " + std::to_string(i)));
            m_Textures.push_back(std::move(texture));
        }

        // Create synchronization primitives
        uint32 imageCount = FRAMES_IN_FLIGHT;
        m_ImageAvailableSemaphores.resize(imageCount);
        m_RenderFinishedSemaphores.resize(imageCount);
        m_InFlightFences.resize(imageCount);
        m_Contexts.resize(imageCount);
        m_TextureViews.resize(m_Textures.size());

        for (uint32 i = 0; i < imageCount; i++) {
            vk::SemaphoreCreateInfo semaphoreInfo;
            m_ImageAvailableSemaphores[i] = m_ParentDevice->GetVulkanDevice().createSemaphore(semaphoreInfo);
            m_RenderFinishedSemaphores[i] = m_ParentDevice->GetVulkanDevice().createSemaphore(semaphoreInfo);

            vk::FenceCreateInfo fenceInfo;
            fenceInfo.setFlags(vk::FenceCreateFlagBits::eSignaled);
            m_InFlightFences[i] = m_ParentDevice->GetVulkanDevice().createFence(fenceInfo);

            m_Contexts[i] = std::make_unique<VulkanRenderContext>(&device, false);
        }

        for (uint32 i = 0; i < m_Textures.size(); i++) {
            TextureViewDesc viewDesc = TextureViewDesc::CreateDefault(m_Textures[i].get(), TextureViewType::RENDER_TARGET,
                                                                      TextureViewDimension::TEXTURE_2D);
            m_TextureViews[i] = std::unique_ptr<VulkanTextureView>(
                static_cast<VulkanTextureView*>(m_ParentDevice->CreateTextureView(viewDesc).release())
            );
        }
    }

    VulkanSurface::~VulkanSurface() {
        m_ParentDevice->GetVulkanDevice().waitIdle();

        // Clean up textures and texture views (one per swapchain image)
        for (uint64 i = 0; i < m_Textures.size(); i++) {
            m_Textures[i].reset();
            m_TextureViews[i].reset();
        }

        // Clean up synchronization primitives and contexts (one per frame in flight)
        for (uint64 i = 0; i < m_ImageAvailableSemaphores.size(); i++) {
            m_Contexts[i].reset();
            m_ParentDevice->GetVulkanDevice().destroySemaphore(m_ImageAvailableSemaphores[i]);
            m_ParentDevice->GetVulkanDevice().destroySemaphore(m_RenderFinishedSemaphores[i]);
            m_ParentDevice->GetVulkanDevice().destroyFence(m_InFlightFences[i]);
        }

        m_ParentDevice->GetVulkanDevice().destroySwapchainKHR(m_Swapchain);
        m_ParentDevice->GetVulkanInstance().destroySurfaceKHR(m_Surface);
    }

    RenderContext* VulkanSurface::BeginFrame() {
        static constexpr auto timeout = std::numeric_limits<uint64>::max();
        // Wait for previous frame to complete
        vk::Result result =
            m_ParentDevice->GetVulkanDevice().waitForFences(1, &m_InFlightFences[m_CurrentFrame], vk::True, timeout);
        if (result == vk::Result::eTimeout) {
            std::println("ERROR: vkWaitForFences timed out!");
            return nullptr;
        }

        auto acquireResult = m_ParentDevice->GetVulkanDevice().acquireNextImageKHR(
            m_Swapchain, timeout, m_ImageAvailableSemaphores[m_CurrentFrame], nullptr);

        if (acquireResult.result != vk::Result::eSuccess) {
            if (acquireResult.result == vk::Result::eErrorOutOfDateKHR) {
                std::println("WARN: Swapchain out of date during acquire, needs resize");
                // Advance to next frame to avoid potentially corrupted semaphore
                m_CurrentFrame = (m_CurrentFrame + 1) % FRAMES_IN_FLIGHT;
                return nullptr;
            }
            if (acquireResult.result == vk::Result::eErrorSurfaceLostKHR) {
                std::println("ERROR: Surface lost!");
                // Advance to next frame to avoid potentially corrupted semaphore
                m_CurrentFrame = (m_CurrentFrame + 1) % FRAMES_IN_FLIGHT;
                return nullptr;
            }
            std::println("ERROR: Failed to acquire next image: {}", static_cast<int>(acquireResult.result));
            return nullptr;
        }

        m_ImageIndex = acquireResult.value;

        // Reset fence after successful acquire
        result = m_ParentDevice->GetVulkanDevice().resetFences(1, &m_InFlightFences[m_CurrentFrame]);
        if (result != vk::Result::eSuccess) {
            std::println("ERROR: Failed to reset fences!");
            return nullptr;
        }

        m_Contexts[m_CurrentFrame]->Reset();
        m_Contexts[m_CurrentFrame]->Begin();
        return m_Contexts[m_CurrentFrame].get();
    }

    void VulkanSurface::ClearScreen(float r, float g, float b) {
        vk::CommandBuffer commandBuffer =
            static_cast<VulkanRenderContext*>(m_Contexts[m_CurrentFrame].get())->GetCommandBuffer();

        vk::ImageMemoryBarrier toColorAttachment;
        toColorAttachment.setSrcAccessMask(vk::AccessFlags());
        toColorAttachment.setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite);
        toColorAttachment.setOldLayout(vk::ImageLayout::eUndefined);
        toColorAttachment.setNewLayout(vk::ImageLayout::eColorAttachmentOptimal);
        toColorAttachment.setSrcQueueFamilyIndex(vk::QueueFamilyIgnored);
        toColorAttachment.setDstQueueFamilyIndex(vk::QueueFamilyIgnored);
        toColorAttachment.setImage(m_Textures[m_ImageIndex]->GetImage());
        toColorAttachment.subresourceRange.setAspectMask(vk::ImageAspectFlagBits::eColor);
        toColorAttachment.subresourceRange.setBaseMipLevel(0);
        toColorAttachment.subresourceRange.setLevelCount(1);
        toColorAttachment.subresourceRange.setBaseArrayLayer(0);
        toColorAttachment.subresourceRange.setLayerCount(1);

        commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe,
                                      vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::DependencyFlags(), 0,
                                      nullptr, 0, nullptr, 1, &toColorAttachment);

        m_Textures[m_ImageIndex]->SetCurrentLayout(TextureLayout::COLOR_ATTACHMENT);

        vk::ClearValue clearColor = vk::ClearColorValue(r, g, b, 1.0f);
        vk::RenderingAttachmentInfo attachmentInfo{};
        attachmentInfo.imageView = m_TextureViews[m_CurrentFrame]->GetVkImageView();
        attachmentInfo.imageLayout = vk::ImageLayout::eColorAttachmentOptimal;
        attachmentInfo.loadOp = vk::AttachmentLoadOp::eClear;
        attachmentInfo.storeOp = vk::AttachmentStoreOp::eStore;
        attachmentInfo.clearValue = clearColor;

        vk::Extent2D extent = {static_cast<uint32>(m_Width), static_cast<uint32>(m_Height)};

        vk::Rect2D renderArea{};
        renderArea.extent = extent;
        renderArea.offset = vk::Offset2D{0, 0};

        vk::RenderingInfo renderingInfo{};
        renderingInfo.renderArea = renderArea;
        renderingInfo.layerCount = 1;
        renderingInfo.colorAttachmentCount = 1;
        renderingInfo.pColorAttachments = &attachmentInfo;

        commandBuffer.beginRendering(renderingInfo);
        commandBuffer.endRendering();
    }

    void VulkanSurface::EndFrame() {
        // Switch to present
        {
            vk::CommandBuffer commandBuffer =
                static_cast<VulkanRenderContext*>(m_Contexts[m_CurrentFrame].get())->GetCommandBuffer();

            vk::ImageMemoryBarrier barrier;
            barrier.setSrcAccessMask(vk::AccessFlagBits::eColorAttachmentWrite);
            barrier.setDstAccessMask(vk::AccessFlagBits::eMemoryRead);
            barrier.setOldLayout(vk::ImageLayout::eUndefined);
            barrier.setNewLayout(vk::ImageLayout::ePresentSrcKHR);
            barrier.setSrcQueueFamilyIndex(vk::QueueFamilyIgnored);
            barrier.setDstQueueFamilyIndex(vk::QueueFamilyIgnored);
            barrier.setImage(m_Textures[m_ImageIndex]->GetImage());
            barrier.subresourceRange.setAspectMask(vk::ImageAspectFlagBits::eColor);
            barrier.subresourceRange.setBaseMipLevel(0);
            barrier.subresourceRange.setLevelCount(1);
            barrier.subresourceRange.setBaseArrayLayer(0);
            barrier.subresourceRange.setLayerCount(1);

            commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eColorAttachmentOutput,
                                          vk::PipelineStageFlagBits::eBottomOfPipe, vk::DependencyFlags(), 0, nullptr,
                                          0, nullptr, 1, &barrier);

            // Update tracked layout state
            m_Textures[m_ImageIndex]->SetCurrentLayout(TextureLayout::PRESENT);

            commandBuffer.end();
        }

        // Submit
        {
            vk::CommandBuffer commandBuffer =
                static_cast<VulkanRenderContext*>(m_Contexts[m_CurrentFrame].get())->GetCommandBuffer();
            vk::Semaphore waitSemaphores[] = {m_ImageAvailableSemaphores[m_CurrentFrame]};
            vk::PipelineStageFlags waitStages[] = {vk::PipelineStageFlagBits::eAllCommands};
            vk::Semaphore signalSemaphores[] = {m_RenderFinishedSemaphores[m_CurrentFrame]};

            vk::SubmitInfo submitInfo;
            submitInfo.setWaitSemaphoreCount(1);
            submitInfo.setPWaitSemaphores(waitSemaphores);
            submitInfo.setPWaitDstStageMask(waitStages);
            submitInfo.setCommandBufferCount(1);
            submitInfo.setPCommandBuffers(&commandBuffer);
            submitInfo.setSignalSemaphoreCount(1);
            submitInfo.setPSignalSemaphores(signalSemaphores);

            vk::Result result = m_ParentDevice->GetMainQueue().submit(1, &submitInfo, m_InFlightFences[m_CurrentFrame]);
            if (result != vk::Result::eSuccess) {
                std::println("ERROR: Failed to submit draw command buffer: {}", vk::to_string(result));
            }
        }

        // Present
        {
            vk::Semaphore waitSemaphores[] = {m_RenderFinishedSemaphores[m_CurrentFrame]};
            vk::SwapchainKHR swapchains[] = {m_Swapchain};

            vk::PresentInfoKHR presentInfo;
            presentInfo.setWaitSemaphoreCount(1);
            presentInfo.setPWaitSemaphores(waitSemaphores);
            presentInfo.setSwapchainCount(1);
            presentInfo.setPSwapchains(swapchains);
            presentInfo.setPImageIndices(&m_ImageIndex);

            vk::Result result = m_ParentDevice->GetMainQueue().presentKHR(presentInfo);
            if (result == vk::Result::eErrorOutOfDateKHR) {
                std::println("WARN: Swapchain out of date during present, needs resize");
            } else if (result == vk::Result::eErrorSurfaceLostKHR) {
                std::println("ERROR: Surface lost!");
            } else if (result != vk::Result::eSuccess) {
                std::println("ERROR: Failed to present swapchain image!");
            }

            m_CurrentFrame = (m_CurrentFrame + 1) % FRAMES_IN_FLIGHT;
        }
    }

    void VulkanSurface::Resize(int width, int height) {
        m_ParentDevice->WaitIdle();

        m_Width = width;
        m_Height = height;

        // Reset frame counter to start fresh after resize
        m_CurrentFrame = 0;

        // Clean up synchronization primitives first - they may be in undefined state after failed acquire
        uint32 imageCount = FRAMES_IN_FLIGHT;
        for (uint32 i = 0; i < imageCount; i++) {
            m_ParentDevice->GetVulkanDevice().destroySemaphore(m_ImageAvailableSemaphores[i]);
            m_ParentDevice->GetVulkanDevice().destroySemaphore(m_RenderFinishedSemaphores[i]);
            m_ParentDevice->GetVulkanDevice().destroyFence(m_InFlightFences[i]);
        }

        // Recreate synchronization primitives in clean state
        for (uint32 i = 0; i < imageCount; i++) {
            vk::SemaphoreCreateInfo semaphoreInfo;
            m_ImageAvailableSemaphores[i] = m_ParentDevice->GetVulkanDevice().createSemaphore(semaphoreInfo);
            m_RenderFinishedSemaphores[i] = m_ParentDevice->GetVulkanDevice().createSemaphore(semaphoreInfo);

            vk::FenceCreateInfo fenceInfo;
            fenceInfo.setFlags(vk::FenceCreateFlagBits::eSignaled);
            m_InFlightFences[i] = m_ParentDevice->GetVulkanDevice().createFence(fenceInfo);
        }

        // Clean up ALL old textures and texture views
        for (uint64 i = 0; i < m_Textures.size(); i++) {
            m_Textures[i].reset();
            m_TextureViews[i].reset();
        }
        m_Textures.clear();
        m_ParentDevice->GetVulkanDevice().destroySwapchainKHR(m_Swapchain);

        auto swapChainSurfaceFormat = chooseSwapSurfaceFormat(m_ParentDevice, m_Surface);
        const auto availablePresentModes =
            m_ParentDevice->GetVulkanPhysicalDevice().getSurfacePresentModesKHR(m_Surface);
        if (!std::ranges::any_of(availablePresentModes,
                                 [](auto presentMode) { return presentMode == vk::PresentModeKHR::eFifo; })) {
            std::unreachable();
        }

        vk::SwapchainCreateInfoKHR swapchainCreateInfo;
        swapchainCreateInfo.setSurface(m_Surface);
        swapchainCreateInfo.setMinImageCount(2);
        swapchainCreateInfo.setImageFormat(swapChainSurfaceFormat.format);
        swapchainCreateInfo.setImageColorSpace(swapChainSurfaceFormat.colorSpace);
        swapchainCreateInfo.setImageExtent(
            vk::Extent2D{static_cast<uint32>(m_Width), static_cast<uint32>(m_Height)});
        swapchainCreateInfo.setImageArrayLayers(1);
        swapchainCreateInfo.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment);
        swapchainCreateInfo.setImageSharingMode(vk::SharingMode::eExclusive);
        swapchainCreateInfo.setPreTransform(vk::SurfaceTransformFlagBitsKHR::eIdentity);
        swapchainCreateInfo.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque);
        swapchainCreateInfo.setPresentMode(vk::PresentModeKHR::eFifo);
        swapchainCreateInfo.setClipped(vk::True);

        m_Swapchain = m_ParentDevice->GetVulkanDevice().createSwapchainKHR(swapchainCreateInfo);

        auto swapchainImages = m_ParentDevice->GetVulkanDevice().getSwapchainImagesKHR(m_Swapchain);

        // Resize vectors to match new swapchain image count
        m_Textures.resize(swapchainImages.size());
        m_TextureViews.resize(swapchainImages.size());

        // Create new textures for all swapchain images
        for (uint64 i = 0; i < swapchainImages.size(); i++) {
            TextureDesc desc;
            desc.Width = width;
            desc.Height = height;
            desc.Format = TextureFormat::RGBA8_SRGB;
            desc.Usage = TextureUsage::RENDER_TARGET;

            m_Textures[i] = std::make_unique<VulkanTexture>(VulkanTexture::AdoptExternal(swapchainImages[i], m_ParentDevice, desc,
                                                                          "Swapchain Texture " + std::to_string(i)));
        }

        // Create texture views for ALL swapchain images
        for (uint32 i = 0; i < swapchainImages.size(); i++) {
            TextureViewDesc viewDesc = TextureViewDesc::CreateDefault(m_Textures[i].get(), TextureViewType::RENDER_TARGET,
                                                                      TextureViewDimension::TEXTURE_2D);
            m_TextureViews[i] = std::unique_ptr<VulkanTextureView>(
                static_cast<VulkanTextureView*>(m_ParentDevice->CreateTextureView(viewDesc).release())
            );
        }
    }
} // namespace Nexus::RHI
