// SPDX-License-Identifier: MIT

export module NE.Engine.RHI.TextureView;

import NE.Engine.Core.Types;
import NE.Engine.RHI.Types;

export namespace Nexus::RHI {
    class TextureView {
    public:
        virtual ~TextureView() = default;

    public:
        virtual TextureHandle GetTexture() = 0;
        virtual Format GetFormat() = 0;
        virtual TextureSubresourceRange GetSubresource() = 0;
    };
} // namespace Nexus::RHI
