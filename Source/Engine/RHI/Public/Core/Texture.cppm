// SPDX-License-Identifier: MIT

module;

#include <Platform/DLL/Export.hpp>

export module NE.Engine.RHI.Texture;

import NE.Engine.Core.Types;

import std;

export namespace Nexus::RHI {
    enum class TextureFormat {
        UNDEFINED,
        R8_UNORM,
        RGBA8_UNORM,
        RGBA8_SRGB,
        BGRA8_UNORM,
        BGRA8_SRGB,
        D32_FLOAT,
        RGBA16_FLOAT,

        // Available everywhere except iOS
        BC7_UNORM,
        BC7_SRGB,

        /// Only supported on macOS
        ASTC6X6_UNORM,
        ASTC6X6_SRGB
    };

    enum class TextureUsage {
        RENDER_TARGET = 1 << 0,
        DEPTH_TARGET = 1 << 1,
        SHADER_READ = 1 << 2,
        SHADER_WRITE = 1 << 3
    };

    enum class TextureLayout {
        UNDEFINED,
        GENERAL,
        READ_ONLY,
        COLOR_ATTACHMENT,
        DEPTH_ATTACHMENT,
        DEPTH_STENCIL_READ,
        DEPTH_STENCIL_WRITE,
        TRANSFER_SRC,
        TRANSFER_DST,
        PRESENT
    };

    struct TextureDesc {
        uint32 Width = 1;
        uint32 Height = 1;
        uint32 Depth = 1;
        uint32 ArrayLayers = 1;
        uint32 MipLevels = 1;
        TextureFormat Format = TextureFormat::RGBA8_SRGB;
        TextureUsage Usage = TextureUsage::SHADER_READ;
        bool IsCubeMap = false;

        TextureDesc& SetSize(uint32 width, uint32 height, uint32 depth = 1) {
            Width = width;
            Height = height;
            Depth = depth;
            return *this;
        }

        TextureDesc& SetArrayLayers(uint32 layers) {
            ArrayLayers = layers;
            return *this;
        }

        TextureDesc& SetMipLevels(uint32 mipLevels) {
            MipLevels = mipLevels;
            return *this;
        }

        TextureDesc& SetFormat(TextureFormat format) {
            Format = format;
            return *this;
        }

        TextureDesc& SetUsage(TextureUsage usage) {
            Usage = usage;
            return *this;
        }
    };

    class NEXUS_API Texture {
    public:
        virtual ~Texture() = default;

        virtual void SetName(const std::string& name) = 0;
        virtual void Resize(uint32 width, uint32 height) = 0;

        const TextureDesc& GetDesc() const {
            return m_Desc;
        }

        TextureLayout GetCurrentLayout() const {
            return m_CurrentLayout;
        }
        void SetCurrentLayout(TextureLayout layout) {
            m_CurrentLayout = layout;
        }

    protected:
        TextureDesc m_Desc;
        TextureLayout m_CurrentLayout = TextureLayout::UNDEFINED;
    };
} // namespace Nexus::RHI

export namespace Nexus {
    template <>
    struct EnableFlagOperators<RHI::TextureUsage> : std::true_type {};
} // namespace Nexus
