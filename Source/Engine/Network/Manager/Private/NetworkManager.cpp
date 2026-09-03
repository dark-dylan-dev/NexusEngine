// SPDX-License-Identifier: MIT

module NE.Engine.Network.Manager;

import NE.Engine.Network.Common.NetworkAddress;
import NE.Engine.Network.Common.NetworkError;
import NE.Engine.Network.Protocol.PacketDispatcher;
import NE.Engine.Network.Packet;
import NE.Engine.Network.PacketWriter;
import NE.Engine.Network.TCP.TCPClient;
import NE.Engine.Network.UDP.UDPSocket;

import NE.Engine.Core.Types;

import std;

namespace Nexus::Network {
    bool NetworkManager::StartClient(const NetworkAddress& server) {
        m_serverAddress = server;

        const bool tcpStarted = m_tcp.Connect(server);
        const bool udpStarted = m_udp.Bind(server.Port()); // binding to 0 for an ephemeral port also works

        m_lastConnectAttempt = std::chrono::steady_clock::now();

        return tcpStarted || udpStarted;
    }

    void NetworkManager::RegisterHandler(PacketType type, PacketDispatcher::Handler handler) {
        m_dispatcher.Register(type, std::move(handler));
    }

    bool NetworkManager::SendReliable(std::vector<byte> framedPacket) {
        return m_tcp.Send(std::move(framedPacket));
    }

    bool NetworkManager::SendUnreliable(std::vector<byte> framedPacket) {
        return m_udp.Send(m_serverAddress, framedPacket);
    }

    PacketWriter NetworkManager::MakeWriter(PacketType type) {
        return PacketWriter(type, m_nextSequence++);
    }

    bool NetworkManager::IsConnected() const noexcept {
        return m_tcp.IsConnected();
    }

    bool NetworkManager::IsConnecting() const noexcept {
        return m_tcp.IsConnecting();
    }

    void NetworkManager::Update() {
        // If we're neither connected nor connecting, try to reconnect periodically.
        const auto now = std::chrono::steady_clock::now();
        if (!IsConnected() && !IsConnecting()) {
            if (now - m_lastConnectAttempt >= m_connectRetryInterval) {
                m_tcp.Connect(m_serverAddress);
                m_lastConnectAttempt = now;
            }
        }

        for (auto& frame : m_tcp.Poll())
            DispatchIncoming(frame);

        std::optional<NetworkError> udpError;
        while (auto message = m_udp.Receive(udpError))
            DispatchIncoming(message->data);
    }

    void NetworkManager::DispatchIncoming(std::span<const byte> frame) const {
        m_dispatcher.Dispatch(frame);
    }
} // namespace Nexus::Network
