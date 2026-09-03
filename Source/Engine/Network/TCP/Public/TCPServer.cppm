// SPDX-License-Identifier: MIT

module;

#include <Platform/DLL/Export.hpp>

export module NE.Engine.Network.TCP.TCPServer;

import NE.Engine.Network.TCP.TCPClient;
import NE.Engine.Network.Platform.Socket;

import NE.Engine.Core.Types;

import std;

export namespace Nexus::Network {
    using ClientId = uint32;

    class NEXUS_API TCPServer {
    public:
        bool Listen(uint16 port);

        void Stop();

        std::vector<ClientId> AcceptPending();

        std::vector<std::pair<ClientId, std::vector<byte>>> PollAll();

        bool SendTo(ClientId id, std::span<const byte> framedData);

        bool SendTo(ClientId id, std::vector<byte>&& framedData);

        void Broadcast(std::span<const byte> framedData);

        [[nodiscard]] usize ClientCount() const noexcept;

    private:
        Socket m_listenSocket;
        std::unordered_map<ClientId, TCPClient> m_clients;
        ClientId m_nextClientId = 1;
    };
} // namespace Nexus::Network
