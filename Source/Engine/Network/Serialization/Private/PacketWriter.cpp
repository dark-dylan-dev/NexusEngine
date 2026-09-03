// SPDX-License-Identifier: MIT

module NE.Engine.Network.PacketWriter;

import NE.Engine.Network.Packet;

import NE.Engine.Core.Types;

import std;

namespace Nexus::Network {
    PacketWriter::PacketWriter(PacketType type, uint32 sequence, usize reserveBytes)
        : m_type(type),
          m_sequence(sequence) {
        m_buffer.reserve(std::max(reserveBytes, HEADER_SIZE));
        m_buffer.resize(HEADER_SIZE);
    }

    bool PacketWriter::WriteString(std::string_view text) {
        if (text.size() > std::numeric_limits<uint32>::max()) {
            return false;
        }

        Write(static_cast<uint32>(text.size()));
        const auto* bytes = reinterpret_cast<const byte*>(text.data());
        m_buffer.insert(m_buffer.end(), bytes, bytes + text.size());
        return true;
    }

    std::vector<byte> PacketWriter::Build() {
        if (m_built) {
            return {};
        }

        const usize payloadSize = m_buffer.size() - HEADER_SIZE;
        if (payloadSize > MAX_PAYLOAD_SIZE) {
            return {};
        }

        usize offset = 0;
        const auto WriteHeaderField = [this, &offset](auto value) {
            const auto wireValue = Wire::HostToBigEndian(value);
            std::memcpy(m_buffer.data() + offset, &wireValue, sizeof(wireValue));
            offset += sizeof(wireValue);
        };

        WriteHeaderField(static_cast<uint32>(payloadSize));
        WriteHeaderField(static_cast<uint16>(m_type));
        WriteHeaderField(m_sequence);

        m_built = true;
        return std::move(m_buffer);
    }

    std::span<const byte> PacketWriter::Data() const noexcept {
        return m_buffer;
    }
} // namespace Nexus::Network
