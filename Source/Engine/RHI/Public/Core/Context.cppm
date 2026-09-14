// SPDX-License-Identifier: MIT

module;

#include <GLM/glm.hpp>
#include <string>
#include <vector>

export module NE.Engine.RHI.Context;

//import NE.Engine.RHI.Buffer;
import NE.Engine.RHI.Texture;
import NE.Engine.RHI.TextureView;

import NE.Engine.Core.Types;

export namespace Nexus::RHI {
    class Buffer;

    enum class PipelineStage : uint64 {
        NONE = 1 << 0,
        TOP_OF_PIPE = 1 << 1,
        DRAW_INDIRECT = 1 << 2,
        VERTEX_INPUT = 1 << 3,
        INDEX_INPUT = 1 << 4,
        VERTEX_SHADER = 1 << 5,
        PIXEL_SHADER = 1 << 6,
        COMPUTE_SHADER = 1 << 7,
        EARLY_FRAGMENT_TESTS = 1 << 8,
        LATE_FRAGMENT_TESTS = 1 << 9,
        COLOR_ATTACHMENT_OUTPUT = 1 << 10,
        BOTTOM_OF_PIPE = 1 << 11,
        COPY = 1 << 12,
        ALL_GRAPHICS = 1 << 13,
        ALL_COMMANDS = 1 << 14,
        ACCELERATION_STRUCTURE_WRITE = 1 << 15,
        ACCELERATION_STRUCTURE_READ = 1 << 16
    };

    enum class ResourceAccess : uint64 {
        NONE = 1 << 0,
        INDIRECT_COMMAND_READ = 1 << 1,
        INDEX_READ = 1 << 2,
        VERTEX_ATTRIBUTE_READ = 1 << 3,
        UNIFORM_READ = 1 << 4,
        SHADER_READ = 1 << 5,
        SHADER_WRITE = 1 << 6,
        COLOR_ATTACHMENT_READ = 1 << 7,
        COLOR_ATTACHMENT_WRITE = 1 << 8,
        DEPTH_STENCIL_ATTACHMENT_READ = 1 << 9,
        DEPTH_STENCIL_ATTACHMENT_WRITE = 1 << 10,
        TRANSFER_READ = 1 << 11,
        TRANSFER_WRITE = 1 << 12,
        HOST_READ = 1 << 13,
        HOST_WRITE = 1 << 14,
        MEMORY_READ = 1 << 15,
        MEMORY_WRITE = 1 << 16,
        ACCELERATION_STRUCTURE_READ = 1 << 17,
        ACCELERATION_STRUCTURE_WRITE = 1 << 18
    };

    struct RenderEncoderInfo {
        std::string Name = "Render Pass";
        std::vector<TextureView*> ColorAttachments;
        std::vector<bool> ColorClearFlags;
        std::vector<glm::vec4> ColorClearValues;

        uint32 Width = 0;
        uint32 Height = 0;

        TextureView* DepthAttachment = nullptr;
        bool DepthClearFlag = true;
        bool DepthStoreFlag = true;

        RenderEncoderInfo& SetName(const std::string& name) {
            Name = name;
            return *this;
        }

        RenderEncoderInfo& AddColorAttachment(TextureView* textureView, bool clearFlag = true,
                                              const glm::vec4& clearValue = glm::vec4(0.0f)) {
            ColorAttachments.push_back(textureView);
            ColorClearFlags.push_back(clearFlag);
            ColorClearValues.push_back(clearValue);
            return *this;
        }

        RenderEncoderInfo& SetDepthAttachment(TextureView* textureView, bool clearFlag = true, bool storeFlag = true) {
            DepthAttachment = textureView;
            DepthClearFlag = clearFlag;
            DepthStoreFlag = storeFlag;
            return *this;
        }

        RenderEncoderInfo& SetDimensions(uint32 width, uint32 height) {
            Width = width;
            Height = height;
            return *this;
        }
    };

    struct TextureBarrier {
        PipelineStage DstStage;
        ResourceAccess DstAccess;
        TextureLayout NewLayout;
        Texture* TargetTexture;

        uint32 BaseMipLevel = 0;
        uint32 MipLevelCount = VIEW_ALL_MIP_LEVELS;
        uint32 BaseArrayLayer = 0;
        uint32 ArrayLayerCount = VIEW_ALL_ARRAY_LAYERS;

        TextureBarrier& SetMipLevels(uint32 baseMipLevel, uint32 mipLevelCount) {
            BaseMipLevel = baseMipLevel;
            MipLevelCount = mipLevelCount;
            return *this;
        }

        TextureBarrier& SetArrayLayers(uint32 baseArrayLayer, uint32 arrayLayerCount) {
            BaseArrayLayer = baseArrayLayer;
            ArrayLayerCount = arrayLayerCount;
            return *this;
        }

        TextureBarrier& SetTargetTexture(Texture* texture) {
            TargetTexture = texture;
            return *this;
        }

        TextureBarrier& SetDstStage(PipelineStage stage) {
            DstStage = stage;
            return *this;
        }

        TextureBarrier& SetDstAccess(ResourceAccess access) {
            DstAccess = access;
            return *this;
        }

        TextureBarrier& SetNewLayout(TextureLayout layout) {
            NewLayout = layout;
            return *this;
        }
    };

    struct BufferBarrier {
        PipelineStage DstStage;
        ResourceAccess DstAccess;
        Buffer* TargetBuffer;

        BufferBarrier& SetTargetBuffer(Buffer* buffer) {
            TargetBuffer = buffer;
            return *this;
        }

        BufferBarrier& SetDstStage(PipelineStage stage) {
            DstStage = stage;
            return *this;
        }

        BufferBarrier& SetDstAccess(ResourceAccess access) {
            DstAccess = access;
            return *this;
        }
    };

    struct BarrierGroup {
        std::vector<TextureBarrier> TextureBarriers;
        std::vector<BufferBarrier> BufferBarriers;
    };

    class RenderEncoder;
    class CopyEncoder;
    class ComputeEncoder;

    class RenderContext {
    public:
        virtual ~RenderContext() = default;

        virtual void Reset() = 0;
        virtual void Begin() = 0;
        virtual void End() = 0;

        virtual void Barrier(const TextureBarrier& textureBarrier) = 0;
        virtual void Barrier(const BufferBarrier& bufferBarrier) = 0;
        virtual void Barrier(const BarrierGroup& barrierGroup) = 0;

        virtual void PushMarker(const std::string& name) = 0;
        virtual void PopMarker() = 0;

        virtual RenderEncoder* CreateRenderEncoder(const RenderEncoderInfo& info) = 0;
        virtual CopyEncoder* CreateCopyEncoder(const std::string& name = "Copy Pass") = 0;
        virtual ComputeEncoder* CreateComputeEncoder(const std::string& name = "Compute Pass") = 0;
    };
} // namespace Nexus::RHI

export namespace Nexus {
    template <>
    struct EnableFlagOperators<RHI::ResourceAccess> : std::true_type {};

    template <>
    struct EnableFlagOperators<RHI::PipelineStage> : std::true_type {};
} // namespace Nexus
