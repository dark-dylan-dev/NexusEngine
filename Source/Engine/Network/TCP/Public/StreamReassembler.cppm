// SPDX-License-Identifier: MIT

module;

#include <Platform/DLL/Export.hpp>

export module NE.Engine.Network.TCP.StreamReassembler;

import NE.Engine.Network.Packet;

import NE.Engine.Core.Types;

import std;

export namespace Nexus::Network {
    class NEXUS_API StreamReassembler {
    public:
        static constexpr usize MAX_BUFFERED_BYTES = MAX_FRAME_SIZE * 2;

        void Feed(std::span<const byte> rawBytes);

        [[nodiscard]] std::optional<std::vector<byte>> TryExtract();

        [[nodiscard]] bool HasInvalidData() const noexcept;

        void Reset() noexcept;

    private:
        [[nodiscard]] usize BufferedSize() const noexcept;

        void Compact();

        std::vector<byte> m_buffer;
        usize m_readPos = 0;
        bool m_invalid = false;
    };
} // namespace Nexus::Network
