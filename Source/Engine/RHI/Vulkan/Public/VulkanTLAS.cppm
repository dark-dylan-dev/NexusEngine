// SPDX-License-Identifier: MIT

export module NE.Engine.RHI.Vulkan.TLAS;

import NE.Engine.Core.Types;
import NE.Engine.RHI.TLAS;
import NE.Engine.RHI.Types;

export namespace Nexus::RHI {
    class VulkanTLAS : public TLAS {
    public:
        void Build(ContextHandle context, BuildDesc desc) override;
        void Update(ContextHandle context, BuildDesc desc) override;
        uint64 GetDeviceAddress() override;
    };
} // namespace Nexus::RHI
