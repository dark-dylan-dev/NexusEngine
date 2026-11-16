// SPDX-License-Identifier: MIT

module;

#include <Platform/DLL/Export.hpp>

export module NE.Engine.RHI.Device;

import NE.Engine.RHI.Surface;
import NE.Engine.RHI.TextureView;

import NE.Engine.Core.Window;

import std;

export namespace Nexus::RHI {
    class NEXUS_API Device {
    public:
        static std::unique_ptr<Device> Create();

        virtual std::unique_ptr<Surface> CreateSurface(const Window& window) = 0;
        virtual std::unique_ptr<TextureView> CreateTextureView(const TextureViewDesc& desc) = 0;

        virtual void WaitIdle() const = 0;

        virtual ~Device() = default;
    };
} // namespace Nexus::RHI
