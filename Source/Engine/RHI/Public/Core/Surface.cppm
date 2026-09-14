// SPDX-License-Identifier: MIT

module;

#include <Platform/DLL/Export.hpp>

export module NE.Engine.RHI.Surface;

import NE.Engine.RHI.Context;

import NE.Engine.Core.Types;

import std;

export namespace Nexus::RHI {
    class NEXUS_API Surface {
    public:
        static constexpr uint32 FRAMES_IN_FLIGHT = 2;
        virtual ~Surface() = default;

        virtual RenderContext* BeginFrame() = 0;
        virtual void ClearScreen(float r, float g, float b) = 0;
        virtual void EndFrame() = 0;

        virtual void Resize(int32 w, int32 h) = 0;
    };
} // namespace Nexus::RHI
