// SPDX-License-Identifier: MIT

export module NE.Engine.RHI.Device;

import NE.Engine.RHI.Types;

export namespace Nexus::RHI {
    class RendererDevice {
    public:
        virtual ~RendererDevice() = default;

    public:
        virtual SurfaceHandle CreateSurface(WindowHandle) = 0;
        virtual void DestroySurface(SurfaceHandle) = 0;

        virtual BufferHandle CreateBuffer(BufferDesc, MemoryUsage) = 0;
        virtual void DestroyBuffer(BufferHandle) = 0;

        virtual TextureHandle CreateTexture(TextureDesc, MemoryUsage) = 0;
        virtual void DestroyTexture(TextureHandle) = 0;

        virtual SamplerHandle CreateSampler(SamplerDesc) = 0;
        virtual void DestroySampler(SamplerHandle) = 0;

        virtual PipelineHandle CreatePipeline(GraphicsPipelineDesc) = 0;
        virtual PipelineHandle CreateComputePipeline(ComputePipelineDesc) = 0;
        virtual void DestroyPipeline(PipelineHandle) = 0;

        virtual ASHandle CreateBLAS(BLASDesc) = 0;
        virtual ASHandle CreateTLAS(TLASDesc) = 0;
        virtual void DestroyAccelerationStructure(ASHandle) = 0;

        virtual ContextHandle CreateContext(QueueType) = 0;
        virtual void DestroyContext(ContextHandle) = 0;

        virtual void WaitIdle() = 0;
    };
} // namespace Nexus::RHI
