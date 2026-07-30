// SPDX-License-Identifier: MIT

export module NE.Engine.RHI.Vulkan.Context;

import NE.Engine.Core.Types;
import NE.Engine.RHI.Context;
import NE.Engine.RHI.Types;

export namespace Nexus::RHI {
    class VulkanContext final : public Context {
    public:
        void Begin() override;
        void End() override;

        void Submit(SubmitInfo submitInfo) override;
        void Wait(FenceHandle fence) override;

        void BeginRenderPass(RenderPassDesc desc) override;
        void EndRenderPass() override;

        void BindPipeline(PipelineHandle pipeline) override;
        void BindVertexBuffer(uint32 slot, BufferHandle buffer, uint64 offset) override;

        void BindIndexBuffer(BufferHandle buffer, uint64 offset, IndexType indexType) override;
        void BindDescriptorSets(PipelineLayout layout, uint32 firstSet, void* descriptorSets) override;
        void Dispatch(uint32 x, uint32 y, uint32 z) override;

        void Draw(uint32 vertexCount, uint32 instanceCount, uint32 firstVertex, uint32 firstInstance) override;
        void DrawIndexed(uint32 indexCount, uint32 instanceCount, uint32 firstIndex, int32 vertexOff,
                         uint32 firstInstance) override;

        void CopyBuffer(BufferHandle src, BufferHandle dst, uint64 size) override;
        void CopyBufferToTexture(BufferHandle src, TextureHandle dst, BufferTextureCopyRegion region) override;
        void CopyTexture(TextureHandle src, TextureHandle dst, TextureCopyRegion region) override;

        void TransitionResource(ResourceHandle resource, ResourceState oldState, ResourceState newState) override;
    };
} // namespace Nexus::RHI
