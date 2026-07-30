// SPDX-License-Identifier: MIT

export module NE.Engine.RHI.Vulkan.Buffer;

import NE.Engine.Core.Types;
import NE.Engine.RHI.Buffer;
import NE.Engine.RHI.Types;

export namespace Nexus::RHI {
    class VulkanBuffer : public Buffer {
    public:
        void* Map(MapMode mode) override;
        void Unmap() override;

        void UpdateData(const void* data, uint64 size, uint64 offset) override;

        uint64 GetSize() override;
        BufferUsage GetUsage() override;
    };
} // namespace Nexus::RHI
