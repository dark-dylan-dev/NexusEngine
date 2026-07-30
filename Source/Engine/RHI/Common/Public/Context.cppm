// SPDX-License-Identifier: MIT

export module NE.Engine.RHI.Context;

import NE.Engine.Core.Types;
import NE.Engine.RHI.Types;

export namespace Nexus::RHI {
    class Context {
    public:
        virtual ~Context() = default;

    public:
        virtual void Begin() = 0;
        virtual void End() = 0;

        virtual void Submit(SubmitInfo submitInfo) = 0;
        virtual void Wait(FenceHandle fence) = 0;

        virtual void BeginRenderPass(RenderPassDesc desc) = 0;
        virtual void EndRenderPass() = 0;

        virtual void BindPipeline(PipelineHandle pipeline) = 0;
        virtual void BindVertexBuffer(uint32 slot, BufferHandle buffer, uint64 offset) = 0;
        virtual void BindIndexBuffer(BufferHandle buffer, uint64 offset, IndexType indexType) = 0;
        virtual void BindDescriptorSets(PipelineLayout layout, uint32 firstSet, void* descriptorSets) = 0;

        virtual void Dispatch(uint32 x, uint32 y, uint32 z) = 0;

        virtual void Draw(uint32 vertexCount, uint32 instanceCount, uint32 firstVertex, uint32 firstInstance) = 0;
        virtual void DrawIndexed(uint32 indexCount, uint32 instanceCount, uint32 firstIndex, int32 vertexOffset,
                                 uint32 firstInstance) = 0;

        virtual void CopyBuffer(BufferHandle src, BufferHandle dst, uint64 size) = 0;
        virtual void CopyBufferToTexture(BufferHandle src, TextureHandle dst, BufferTextureCopyRegion region) = 0;
        virtual void CopyTexture(TextureHandle src, TextureHandle dst, TextureCopyRegion region) = 0;

        virtual void TransitionResource(ResourceHandle resource, ResourceState oldState, ResourceState newState) = 0;
    };
} // namespace Nexus::RHI
