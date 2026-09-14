// SPDX-License-Identifier: MIT

module;

#include <Platform/DLL/Export.hpp>

export module NE.Engine.RHI.Vulkan.Texture;

import NE.Engine.RHI.Texture;

import NE.Engine.RHI.Vulkan.Device;

import NE.Engine.Core.Types;
import NE.Engine.Core.Window;

import vulkan;
import vk_mem_alloc;

import std;

export namespace Nexus::RHI {
    class NEXUS_API VulkanTexture final : public Texture {
    public:
        VulkanTexture() = default;
        VulkanTexture(const TextureDesc& desc, VulkanDevice* device, vma::Allocator allocator);
        ~VulkanTexture() override;

        void SetName(const std::string& name) override;
        void Resize(uint32 width, uint32 height) override;

        static VulkanTexture AdoptExternal(vk::Image image, VulkanDevice* device, const TextureDesc& desc,
                                           const std::string& name);

        vma::Allocation GetAllocation() const {
            return m_Allocation;
        }
        vk::Image GetImage() const {
            return m_Image;
        }

    private:
        vma::Allocation m_Allocation = nullptr;
        vma::AllocationInfo m_AllocationInfo;
        vk::Image m_Image = nullptr;

        VulkanDevice* m_ParentDevice = nullptr;
        vma::Allocator m_ParentAllocator = nullptr;

        bool m_ShouldDestroy = true;
    };

    inline vk::Format ConvertTextureFormatToVulkan(TextureFormat format) {
        switch (format) {
            case TextureFormat::UNDEFINED:
                return vk::Format::eUndefined;
            case TextureFormat::R8_UNORM:
                return vk::Format::eR8Unorm;
            case TextureFormat::RGBA8_UNORM:
                return vk::Format::eR8G8B8A8Unorm;
            case TextureFormat::RGBA8_SRGB:
                return vk::Format::eR8G8B8A8Srgb;
            case TextureFormat::BGRA8_UNORM:
                return vk::Format::eB8G8R8A8Unorm;
            case TextureFormat::BGRA8_SRGB:
                return vk::Format::eB8G8R8A8Srgb;
            case TextureFormat::ASTC6X6_SRGB:
                return vk::Format::eAstc6x6SrgbBlock;
            case TextureFormat::ASTC6X6_UNORM:
                return vk::Format::eAstc6x6UnormBlock;
            case TextureFormat::BC7_SRGB:
                return vk::Format::eBc7SrgbBlock;
            case TextureFormat::BC7_UNORM:
                return vk::Format::eBc7UnormBlock;
            case TextureFormat::D32_FLOAT:
                return vk::Format::eD32Sfloat;
            case TextureFormat::RGBA16_FLOAT:
                return vk::Format::eR16G16B16A16Sfloat;
            default:
                return vk::Format::eUndefined;
        }
    }

    inline vk::ImageUsageFlags ConvertTextureUsageToVulkan(TextureUsage usage) {
        vk::ImageUsageFlags flags = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eTransferSrc;
        if (HasFlag(usage, TextureUsage::RENDER_TARGET))
            flags |= vk::ImageUsageFlagBits::eColorAttachment;
        if (HasFlag(usage, TextureUsage::DEPTH_TARGET))
            flags |= vk::ImageUsageFlagBits::eDepthStencilAttachment;
        if (HasFlag(usage, TextureUsage::SHADER_READ))
            flags |= vk::ImageUsageFlagBits::eSampled;
        if (HasFlag(usage, TextureUsage::SHADER_WRITE))
            flags |= vk::ImageUsageFlagBits::eStorage;
        return flags;
    }
} // namespace Nexus::RHI
