// SPDX-License-Identifier: MIT

export module NE.Engine.RHI.Pipeline;

import NE.Engine.Core.Types;
import NE.Engine.RHI.Types;

export namespace Nexus::RHI {
    class Pipeline {
    public:
        virtual ~Pipeline() = default;

    public:
        virtual PipelineLayout GetLayout() = 0;
        virtual void Bind(ContextHandle context) = 0;
    };
} // namespace Nexus::RHI
