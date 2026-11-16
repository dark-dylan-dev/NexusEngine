// SPDX-License-Identifier: MIT

module;

#include <Platform/DLL/Export.hpp>

export module NE.Engine.RHI.Vulkan.TextureView;

import NE.Engine.RHI.Vulkan.Device;
import NE.Engine.RHI.TextureView;

import NE.Engine.Core.Types;

import vulkan;

export namespace Nexus::RHI {
    class NEXUS_API VulkanTextureView final : public TextureView {
    public:
        VulkanTextureView(VulkanDevice* device, const TextureViewDesc& desc);
        ~VulkanTextureView() override;

        int32 GetBindlessIndex() override {
            return m_BindlessIndex;
        }
        vk::ImageView GetVkImageView() const {
            return m_ImageView;
        }

    private:
        VulkanDevice* m_Device;

        vk::ImageView m_ImageView;
        int32 m_BindlessIndex = -1;
    };

    inline vk::ImageViewType GetVulkanImageViewType(TextureViewDimension type) {
        switch (type) {
            case TextureViewDimension::TEXTURE_2D:
                return vk::ImageViewType::e2D;
            case TextureViewDimension::TEXTURE_2D_ARRAY:
                return vk::ImageViewType::e2DArray;
            case TextureViewDimension::TEXTURE_CUBE:
                return vk::ImageViewType::eCube;
            case TextureViewDimension::TEXTURE_3D:
                return vk::ImageViewType::e3D;
        }
        return vk::ImageViewType::e2D;
    }
} // namespace Nexus::RHI
