// SPDX-License-Identifier: MIT

module NE.Engine.RHI.Vulkan.Context;

namespace Nexus::RHI {
    VulkanRenderContext::VulkanRenderContext(VulkanDevice* device, bool singleTime)
        : m_SingleTime(singleTime),
          m_ParentDevice(device) {
        vk::CommandBufferAllocateInfo allocInfo{};
        allocInfo.commandPool = device->GetCommandPool();
        allocInfo.level = vk::CommandBufferLevel::ePrimary;
        allocInfo.commandBufferCount = 1;

        m_CommandBuffer = m_ParentDevice->GetVulkanDevice().allocateCommandBuffers(allocInfo).front();
    }

    VulkanRenderContext::~VulkanRenderContext() {
        m_ParentDevice->GetVulkanDevice().freeCommandBuffers(m_ParentDevice->GetCommandPool(), m_CommandBuffer);
    }

    void VulkanRenderContext::Reset() {
        m_CommandBuffer.reset();
    }

    void VulkanRenderContext::Begin() {
        m_CommandBuffer.begin(m_SingleTime ? vk::CommandBufferBeginInfo{vk::CommandBufferUsageFlagBits::eOneTimeSubmit}
                                           : vk::CommandBufferBeginInfo{});
    }

    void VulkanRenderContext::End() {
        m_CommandBuffer.end();
    }
} // namespace Nexus::RHI
