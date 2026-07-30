// SPDX-License-Identifier: MIT

export module NE.Engine.RHI.Vulkan.Sampler;

import NE.Engine.Core.Types;
import NE.Engine.RHI.Sampler;
import NE.Engine.RHI.Types;

export namespace Nexus::RHI {
    class VulkanSampler : public Sampler {
    public:
        SamplerDesc GetDesc() override;
    };
} // namespace Nexus::RHI
