// SPDX-License-Identifier: MIT

export module NE.Engine.RHI.BLAS;

import NE.Engine.Core.Types;
import NE.Engine.RHI.Types;

export namespace Nexus::RHI {
    class BLAS {
    public:
        virtual ~BLAS() = default;

    public:
        virtual void Build(ContextHandle context, BuildDesc desc) = 0;
        virtual void Update(ContextHandle context, BuildDesc desc) = 0;
        virtual uint64 GetDeviceAddress() = 0;
    };
} // namespace Nexus::RHI
