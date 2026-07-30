// SPDX-License-Identifier: MIT

export module NE.Engine.RHI.Vulkan.BufferView;

import NE.Engine.Core.Types;
import NE.Engine.RHI.BufferView;
import NE.Engine.RHI.Types;

export namespace Nexus::RHI {
    class VulkanBufferView : public BufferView {
    public:
        BufferHandle GetBuffer() override;
        Format GetFormat() override;
        BufferRange GetRange() override;
    };
} // namespace Nexus::RHI
