// SPDX-License-Identifier: MIT

module NE.Engine.Network.Protocol.PacketDispatcher;

import NE.Engine.Network.Packet;

import NE.Engine.Core.Types;

import std;

namespace Nexus::Network {
    void PacketDispatcher::Register(PacketType type, Handler handler) {
        if (!handler) {
            m_handlers.erase(type);
            return;
        }
        m_handlers.insert_or_assign(type, std::move(handler));
    }

    void PacketDispatcher::Unregister(PacketType type) {
        m_handlers.erase(type);
    }

    void PacketDispatcher::Clear() {
        m_handlers.clear();
    }

    bool PacketDispatcher::HasHandler(PacketType type) const {
        return m_handlers.contains(type);
    }

    void PacketDispatcher::Dispatch(std::span<const byte> frame) const {
        if (frame.size() < HEADER_SIZE) {
            return;
        }

        usize offset = 0;
        uint32 payloadSize = 0;
        uint16 typeValue = 0;
        uint32 sequence = 0;

        if (!Wire::Read(frame, offset, payloadSize) || !Wire::Read(frame, offset, typeValue) ||
            !Wire::Read(frame, offset, sequence)) {
            return;
        }

        if (payloadSize > MAX_PAYLOAD_SIZE || static_cast<usize>(payloadSize) != frame.size() - HEADER_SIZE) {
            return;
        }

        if (typeValue > static_cast<uint16>(PacketType::ChatMessage)) {
            return;
        }

        const PacketType type = static_cast<PacketType>(typeValue);
        const auto it = m_handlers.find(type);
        if (it == m_handlers.end()) {
            return;
        }

        std::vector<byte> payload(frame.begin() + static_cast<isize>(HEADER_SIZE), frame.end());

        Packet packet(payload);
        it->second(packet);
    }

    void PacketDispatcher::Dispatch(const std::vector<byte>& frame) const {
        Dispatch(std::span<const byte>(frame));
    }
} // namespace Nexus::Network
