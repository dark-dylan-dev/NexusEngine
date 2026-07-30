// SPDX-License-Identifier: MIT

export module NE.Engine.RHI.ComputePipeline;

import NE.Engine.Core.Types;
import NE.Engine.RHI.Types;

export namespace Nexus::RHI {
    class ComputePipeline {
    public:
        virtual ~ComputePipeline() = default;

    public:
        virtual PipelineLayout GetLayout() = 0;
        virtual void Bind(ContextHandle context) = 0;
    };
} // namespace Nexus::RHI
