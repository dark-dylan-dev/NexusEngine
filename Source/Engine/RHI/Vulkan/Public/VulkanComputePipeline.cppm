// SPDX-License-Identifier: MIT

export module NE.Engine.RHI.Vulkan.ComputePipeline;

import NE.Engine.Core.Types;
import NE.Engine.RHI.ComputePipeline;
import NE.Engine.RHI.Types;

export namespace Nexus::RHI {
    class VulkanComputePipeline final : public ComputePipeline {
    public:
        PipelineLayout GetLayout() override;
        void Bind(ContextHandle context) override;
    };
} // namespace Nexus::RHI
