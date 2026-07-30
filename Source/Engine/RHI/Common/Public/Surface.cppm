// SPDX-License-Identifier: MIT

export module NE.Engine.RHI.Surface;

import NE.Engine.Core.Types;
import NE.Engine.RHI.Types;

export namespace Nexus::RHI {
    class Surface {
    public:
        virtual ~Surface() = default;

    public:
        virtual uint32 AcquireNextImage(SemaphoreHandle semaphore, FenceHandle fence) = 0;
        virtual void Present(QueueHandle queue, uint32 imageIndex, SemaphoreHandle semaphore) = 0;

        virtual void Resize(uint32 width, uint32 height) = 0;
        virtual uint32 GetCurrentImageIndex() = 0;
    };
} // namespace Nexus::RHI
