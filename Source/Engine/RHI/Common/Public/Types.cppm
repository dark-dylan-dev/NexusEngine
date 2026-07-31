export module NE.Engine.RHI.Types;

import NE.Engine.Core.Types;
import std;

export namespace Nexus::RHI {

    // Handles
    inline constexpr uint64 INVALID_HANDLE = static_cast<uint64>(-1);
    using BufferHandle = uint64;
    using TextureHandle = uint64;
    using PipelineHandle = uint64;
    using SamplerHandle = uint64;
    using SurfaceHandle = uint64;
    using ContextHandle = uint64;
    using ASHandle = uint64;
    using FenceHandle = uint64;
    using SemaphoreHandle = uint64;
    using QueueHandle = uint64;
    using ResourceHandle = uint64;
    // Enums
    enum class MemoryUsage { Unknown, GpuOnly, CpuToGpu, GpuToCpu, CpuOnly };
    enum class QueueType { Graphics, Compute, Transfer };

    enum class BufferUsage : uint32 {
        None = 0,
        Vertex = 1 << 0,
        Index = 1 << 1,
        Uniform = 1 << 2,
        Storage = 1 << 3,
        Indirect = 1 << 4,
        TransferSrc = 1 << 5,
        TransferDst = 1 << 6
    };

    enum class TextureUsage : uint32 {
        None = 0,
        Sampled = 1 << 0,
        Storage = 1 << 1,
        RenderTarget = 1 << 2,
        DepthStencil = 1 << 3,
        TransferSrc = 1 << 4,
        TransferDst = 1 << 5
    };

    enum class Format {
        Unknown,
        RGBA8_UNORM,
        RGBA8_SRGB,
        BGRA8_UNORM,
        R32_FLOAT,
        RG32_FLOAT,
        RGBA32_FLOAT,
        D24_UNORM_S8_UINT,
        D32_FLOAT
    };

    enum class IndexType { Uint16, Uint32 };
    enum class MapMode { Read, Write, ReadWrite };

    enum class ResourceState {
        Undefined,
        CopySrc,
        CopyDst,
        VertexBuffer,
        IndexBuffer,
        ConstantBuffer,
        ShaderRead,
        ShaderWrite,
        RenderTarget,
        DepthWrite,
        DepthRead,
        Present
    };

    enum class ShaderStage : uint32 {
        None = 0,
        Vertex = 1 << 0,
        Fragment = 1 << 1,
        Compute = 1 << 2,
        AllGraphics = Vertex | Fragment,
        All = 0xFFFFFFFF
    };

    enum class PrimitiveTopology { PointList, LineList, LineStrip, TriangleList, TriangleStrip };

    // Structs
    struct BufferDesc {
        uint64 Size;
        BufferUsage Usage;
    };

    struct TextureDesc {
        uint32 Width;
        uint32 Height;
        uint32 Depth;
        Format TexFormat;
        TextureUsage Usage;
    };

    struct BufferTextureCopyRegion {};
    struct BufferRange {};
    struct BuildDesc {};
    struct BLASDesc {};
    struct ComputePipelineDesc {};
    struct GraphicsPipelineDesc {};
    struct PipelineLayout {};
    struct RenderPassDesc {};
    struct SamplerDesc {};
    struct SubmitInfo {};
    struct TextureCopyRegion {};
    struct TextureSubresourceRange {};
    struct TLASDesc {};
    struct WindowHandle {};

} // namespace Nexus::RHI

template <>
struct Nexus::EnableFlagOperators<Nexus::RHI::BufferUsage> : std::true_type {};
template <>
struct Nexus::EnableFlagOperators<Nexus::RHI::TextureUsage> : std::true_type {};
template <>
struct Nexus::EnableFlagOperators<Nexus::RHI::ShaderStage> : std::true_type {};
