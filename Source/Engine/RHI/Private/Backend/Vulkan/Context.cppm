// SPDX-License-Identifier: MIT

module;

#include <Platform/DLL/Export.hpp>

export module NE.Engine.RHI.Vulkan.Context;

import NE.Engine.RHI.Context;
import NE.Engine.RHI.Vulkan.Device;

import vulkan;

import std;

export namespace Nexus::RHI {
    class NEXUS_API VulkanRenderContext final : public RenderContext {
    public:
        VulkanRenderContext(VulkanDevice* device, bool singleTime = false);
        ~VulkanRenderContext() override;

        void Reset() override;
        void Begin() override;
        void End() override;

        void Barrier(const TextureBarrier& textureBarrier) override {}
        void Barrier(const BufferBarrier& bufferBarrier) override {}
        void Barrier(const BarrierGroup& barrierGroup) override {}

        void PushMarker(const std::string& name) override {}
        void PopMarker() override {}

        RenderEncoder* CreateRenderEncoder(const RenderEncoderInfo& info) override {
            return {};
        }
        CopyEncoder* CreateCopyEncoder(const std::string& name = "Copy Pass") override {
            return {};
        }
        ComputeEncoder* CreateComputeEncoder(const std::string& name = "Compute Pass") override {
            return {};
        }

        vk::CommandBuffer& GetCommandBuffer() {
            return m_CommandBuffer;
        }
        VulkanDevice* GetParentDevice() const {
            return m_ParentDevice;
        }

    private:
        bool m_SingleTime = false;

        VulkanDevice* m_ParentDevice;
        vk::CommandBuffer m_CommandBuffer;
    };
} // namespace Nexus::RHI
