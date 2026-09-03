// SPDX-License-Identifier: MIT

module NE.Engine.Network.Packet;

import NE.Engine.Core.Types;

import std;

namespace Nexus::Network {
    bool Packet::ReadString(std::string& out) {
        uint32 length = 0;
        if (!Read(length)) {
            return false;
        }

        if (static_cast<usize>(length) > m_storage.size() - m_readPos) {
            return false;
        }

        const auto* chars = reinterpret_cast<const char*>(m_storage.data() + m_readPos);
        out.assign(chars, length);
        m_readPos += length;
        return true;
    }

    std::span<const byte> Packet::Data() const noexcept {
        return m_storage;
    }

    usize Packet::Remaining() const noexcept {
        return m_storage.size() - std::min(m_readPos, m_storage.size());
    }

    void Packet::Reset() noexcept {
        m_readPos = 0;
    }
} // namespace Nexus::Network
