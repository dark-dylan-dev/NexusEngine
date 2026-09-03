// SPDX-License-Identifier: MIT

module NE.Engine.Network.TCP.StreamReassembler;

import NE.Engine.Network.Packet;

import NE.Engine.Core.Types;

import std;

namespace Nexus::Network {
    void StreamReassembler::Feed(std::span<const byte> rawBytes) {
        if (m_invalid || rawBytes.empty()) {
            return;
        }

        if (rawBytes.size() > MAX_BUFFERED_BYTES - BufferedSize()) {
            m_buffer.clear();
            m_readPos = 0;
            m_invalid = true;
            return;
        }

        m_buffer.insert(m_buffer.end(), rawBytes.begin(), rawBytes.end());
        m_invalid = false;
    }

    std::optional<std::vector<byte>> StreamReassembler::TryExtract() {
        if (m_invalid) {
            return std::nullopt;
        }

        const std::span<const byte> pending = std::span(m_buffer).subspan(m_readPos);

        if (pending.size() < HEADER_SIZE) {
            Compact();
            return std::nullopt;
        }

        usize offset = 0;
        uint32 payloadSize = 0;
        uint16 typeValue = 0;
        uint32 sequence = 0;

        if (!Wire::Read(pending, offset, payloadSize) || !Wire::Read(pending, offset, typeValue) ||
            !Wire::Read(pending, offset, sequence)) {
            return std::nullopt;
        }

        static_cast<void>(sequence);

        if (payloadSize > MAX_PAYLOAD_SIZE) {
            m_buffer.clear();
            m_readPos = 0;
            m_invalid = true;
            return std::nullopt;
        }

        const usize totalFrameSize = HEADER_SIZE + static_cast<usize>(payloadSize);
        if (pending.size() < totalFrameSize) {
            return std::nullopt;
        }

        std::vector<byte> frame(pending.begin(), pending.begin() + static_cast<isize>(totalFrameSize));

        m_readPos += totalFrameSize;
        Compact();
        return frame;
    }

    bool StreamReassembler::HasInvalidData() const noexcept {
        return m_invalid;
    }

    void StreamReassembler::Reset() noexcept {
        m_buffer.clear();
        m_readPos = 0;
        m_invalid = false;
    }

    usize StreamReassembler::BufferedSize() const noexcept {
        return m_buffer.size() - std::min(m_readPos, m_buffer.size());
    }

    void StreamReassembler::Compact() {
        if (m_readPos == 0) {
            return;
        }

        if (m_readPos == m_buffer.size()) {
            m_buffer.clear();
            m_readPos = 0;
            return;
        }

        if (m_readPos >= 4096 || m_readPos * 2 >= m_buffer.size()) {
            m_buffer.erase(m_buffer.begin(), m_buffer.begin() + static_cast<isize>(m_readPos));
            m_readPos = 0;
        }
    }
} // namespace Nexus::Network
