// SPDX-License-Identifier: MIT

export module NE.Engine.RHI.Vulkan.Texture;

import NE.Engine.Core.Types;
import NE.Engine.RHI.Texture;
import NE.Engine.RHI.Types;

export namespace Nexus::RHI {
    class VulkanTexture : public Texture {
    public:
        uint32 GetWidth() override;
        uint32 GetHeight() override;
        Format GetFormat() override;

        void UploadData(const void* data, uint64 size, uint32 mipLevel, uint32 arrayLayer) override;
        void GenerateMips() override;
    };
} // namespace Nexus::RHI
