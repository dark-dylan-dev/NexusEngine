// SPDX-License-Identifier: MIT

export module NE.Engine.RHI.Texture;

import NE.Engine.Core.Types;
import NE.Engine.RHI.Types;

export namespace Nexus::RHI {
    class Texture {
    public:
        virtual ~Texture() = default;

    public:
        virtual uint32 GetWidth() = 0;
        virtual uint32 GetHeight() = 0;
        virtual Format GetFormat() = 0;

        virtual void UploadData(const void* data, uint64 size, uint32 mipLevel, uint32 arrayLayer) = 0;
        virtual void GenerateMips() = 0;
    };
} // namespace Nexus::RHI
