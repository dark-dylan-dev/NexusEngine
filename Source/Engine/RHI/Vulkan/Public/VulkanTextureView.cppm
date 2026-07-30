// SPDX-License-Identifier: MIT

export module NE.Engine.RHI.Vulkan.TextureView;

import NE.Engine.Core.Types;
import NE.Engine.RHI.TextureView;
import NE.Engine.RHI.Types;

export namespace Nexus::RHI {
    class VulkanTextureView : public TextureView {
    public:
        TextureHandle GetTexture() override;
        Format GetFormat() override;
        TextureSubresourceRange GetSubresource() override;
    };
} // namespace Nexus::RHI
