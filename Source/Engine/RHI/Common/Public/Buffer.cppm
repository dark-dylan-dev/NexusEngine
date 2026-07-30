// SPDX-License-Identifier: MIT

export module NE.Engine.RHI.Buffer;

import NE.Engine.Core.Types;
import NE.Engine.RHI.Types;

export namespace Nexus::RHI {
    class Buffer {
    public:
        virtual ~Buffer() = default;

    public:
        virtual void* Map(MapMode mode) = 0;
        virtual void Unmap() = 0;

        virtual void UpdateData(const void* data, uint64 size, uint64 offset) = 0;

        virtual uint64 GetSize() = 0;
        virtual BufferUsage GetUsage() = 0;
    };
} // namespace Nexus::RHI
