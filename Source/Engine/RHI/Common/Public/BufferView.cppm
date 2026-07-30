// SPDX-License-Identifier: MIT

export module NE.Engine.RHI.BufferView;

import NE.Engine.Core.Types;
import NE.Engine.RHI.Types;

export namespace Nexus::RHI {
    class BufferView {
    public:
        virtual ~BufferView() = default;

    public:
        virtual BufferHandle GetBuffer() = 0;
        virtual Format GetFormat() = 0;
        virtual BufferRange GetRange() = 0;
    };
} // namespace Nexus::RHI
