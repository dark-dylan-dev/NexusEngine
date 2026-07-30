// SPDX-License-Identifier: MIT

export module NE.Engine.RHI.Vulkan.Surface;

import NE.Engine.Core.Types;
import NE.Engine.RHI.Surface;
import NE.Engine.RHI.Types;

export namespace Nexus::RHI {
    class VulkanSurface : public Surface {
    public:
        uint32 AcquireNextImage(SemaphoreHandle semaphore, FenceHandle fence) override;
        void Present(QueueHandle queue, uint32 imageIndex, SemaphoreHandle semaphore) override;

        void Resize(uint32 width, uint32 height) override;
        uint32 GetCurrentImageIndex() override;
    };
} // namespace Nexus::RHI
