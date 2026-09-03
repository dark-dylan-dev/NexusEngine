// SPDX-License-Identifier: MIT

module;

#include <Platform/DLL/Export.hpp>

export module NE.Engine.Network.PacketWriter;

import NE.Engine.Network.Packet;

import NE.Engine.Core.Types;

import std;

export namespace Nexus::Network {
    class NEXUS_API PacketWriter {
    public:
        explicit PacketWriter(PacketType type, uint32 sequence, usize reserveBytes = 64);

        PacketWriter(const PacketWriter&) = delete;
        PacketWriter& operator=(const PacketWriter&) = delete;
        PacketWriter(PacketWriter&&) = default;
        PacketWriter& operator=(PacketWriter&&) = default;

        template <typename T>
        void Write(const T& value);

        bool WriteString(std::string_view text);

        /// Fills in the 10-byte header (payload size, type, sequence) and returns
        /// the completed frame. Can only be called once.
        [[nodiscard]] std::vector<byte> Build();

        [[nodiscard]] std::span<const byte> Data() const noexcept;

    private:
        std::vector<byte> m_buffer;
        PacketType m_type;
        uint32 m_sequence;
        bool m_built = false;
    };
} // namespace Nexus::Network

#include "PacketWriter.inl"
