// SPDX-License-Identifier: MIT

export module NE.Engine.RHI.Vulkan.BLAS;

import NE.Engine.Core.Types;
import NE.Engine.RHI.BLAS;
import NE.Engine.RHI.Types;

export namespace Nexus::RHI {
    class VulkanBLAS : public BLAS {
    public:
        void Build(ContextHandle context, BuildDesc desc) override;
        void Update(ContextHandle context, BuildDesc desc) override;
        uint64 GetDeviceAddress() override;
    };
} // namespace Nexus::RHI
