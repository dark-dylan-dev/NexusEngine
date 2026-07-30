// SPDX-License-Identifier: MIT

export module NE.Engine.RHI.Vulkan.Device;

import NE.Engine.RHI.Device;
import NE.Engine.RHI.Types;

import vk_mem_alloc;

export namespace Nexus::RHI {
    class VulkanDevice final : public RendererDevice {
    public:
        SurfaceHandle CreateSurface(WindowHandle) override;
        void DestroySurface(SurfaceHandle) override;

        BufferHandle CreateBuffer(BufferDesc, MemoryUsage) override;
        void DestroyBuffer(BufferHandle) override;

        TextureHandle CreateTexture(TextureDesc, MemoryUsage) override;
        void DestroyTexture(TextureHandle) override;

        SamplerHandle CreateSampler(SamplerDesc) override;
        void DestroySampler(SamplerHandle) override;

        PipelineHandle CreatePipeline(GraphicsPipelineDesc) override;
        PipelineHandle CreateComputePipeline(ComputePipelineDesc) override;
        void DestroyPipeline(PipelineHandle) override;

        ASHandle CreateBLAS(BLASDesc) override;
        ASHandle CreateTLAS(TLASDesc) override;
        void DestroyAccelerationStructure(ASHandle) override;

        ContextHandle CreateContext(QueueType) override;
        void DestroyContext(ContextHandle) override;

        void WaitIdle() override;

    private:
        void createInstance();

    private:
        vma::raii::Allocator allocator = nullptr;
        vk::raii::Context m_Context;
        vk::raii::Instance m_Instance = nullptr;
        vk::DebugUtilsMessengerEXT m_DebugMessenger;
    };
} // namespace Nexus::RHI
