// SPDX-License-Identifier: MIT

export module NE.Engine.RHI.Vulkan.Pipeline;

import NE.Engine.Core.Types;
import NE.Engine.RHI.Pipeline;
import NE.Engine.RHI.Types;

export namespace Nexus::RHI {
    class VulkanPipeline : public Pipeline {
    public:
        PipelineLayout GetLayout() override;
        void Bind(ContextHandle context) override;
    };
} // namespace Nexus::RHI
