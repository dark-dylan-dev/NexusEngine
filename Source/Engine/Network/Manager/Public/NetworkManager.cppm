// SPDX-License-Identifier: MIT

module;

#include <Platform/DLL/Export.hpp>

export module NE.Engine.Network.Manager;

import NE.Engine.Network.Common.NetworkAddress;
import NE.Engine.Network.Protocol.PacketDispatcher;
import NE.Engine.Network.Packet;
import NE.Engine.Network.PacketWriter;
import NE.Engine.Network.TCP.TCPClient;
import NE.Engine.Network.UDP.UDPSocket;

import NE.Engine.Core.Types;

import std;

export namespace Nexus::Network {
    class NEXUS_API NetworkManager {
    public:
        bool StartClient(const NetworkAddress& server);

        void RegisterHandler(PacketType type, PacketDispatcher::Handler handler);

        [[nodiscard]] bool SendReliable(std::vector<byte> framedPacket);

        [[nodiscard]] bool SendUnreliable(std::vector<byte> framedPacket);

        [[nodiscard]] PacketWriter MakeWriter(PacketType type);

        [[nodiscard]] bool IsConnected() const noexcept;

        [[nodiscard]] bool IsConnecting() const noexcept;

        /// Call once per frame from the main loop.
        void Update();

    private:
        void DispatchIncoming(std::span<const byte> frame) const;

    private:
        TCPClient m_tcp;
        UDPSocket m_udp;
        NetworkAddress m_serverAddress;
        PacketDispatcher m_dispatcher;
        uint32 m_nextSequence = 0;

        // Reconnection state
        std::chrono::steady_clock::time_point m_lastConnectAttempt = std::chrono::steady_clock::now();
        std::chrono::milliseconds m_connectRetryInterval = std::chrono::milliseconds(1000);
    };
} // namespace Nexus::Network
