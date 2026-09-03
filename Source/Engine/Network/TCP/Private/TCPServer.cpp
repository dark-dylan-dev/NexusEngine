// SPDX-License-Identifier: MIT

module NE.Engine.Network.TCP.TCPServer;

import NE.Engine.Network.TCP.TCPClient;
import NE.Engine.Network.Platform.Socket;

import NE.Engine.Core.Types;

import std;

namespace Nexus::Network {
    bool TCPServer::Listen(uint16 port) {
        m_clients.clear();
        m_listenSocket.Close();

        m_listenSocket = Socket(SocketType::TCP);
        if (!m_listenSocket.IsValid()) {
            return false;
        }

        if (!m_listenSocket.SetNonBlocking(true)) {
            m_listenSocket.Close();
            return false;
        }

        if (!m_listenSocket.Bind(port) || !m_listenSocket.Listen()) {
            m_listenSocket.Close();
            return false;
        }

        m_nextClientId = 1;
        return true;
    }

    void TCPServer::Stop() {
        m_listenSocket.Close();
        m_clients.clear();
    }

    std::vector<ClientId> TCPServer::AcceptPending() {
        std::vector<ClientId> newClients;
        if (!m_listenSocket.IsValid()) {
            return newClients;
        }

        while (auto accepted = m_listenSocket.Accept()) {
            const ClientId id = m_nextClientId++;
            m_clients.emplace(id, TCPClient(std::move(*accepted)));
            newClients.push_back(id);
        }

        return newClients;
    }

    std::vector<std::pair<ClientId, std::vector<byte>>> TCPServer::PollAll() {
        std::vector<std::pair<ClientId, std::vector<byte>>> allFrames;
        std::vector<ClientId> toRemove;

        for (auto& [id, client] : m_clients) {
            for (auto& frame : client.Poll()) {
                allFrames.emplace_back(id, std::move(frame));
            }

            if (!client.IsConnected()) {
                toRemove.push_back(id);
            }
        }

        for (const ClientId id : toRemove) {
            m_clients.erase(id);
        }

        return allFrames;
    }

    bool TCPServer::SendTo(ClientId id, std::span<const byte> framedData) {
        const auto it = m_clients.find(id);
        return it != m_clients.end() && it->second.Send(framedData);
    }

    bool TCPServer::SendTo(ClientId id, std::vector<byte>&& framedData) {
        const auto it = m_clients.find(id);
        return it != m_clients.end() && it->second.Send(std::move(framedData));
    }

    void TCPServer::Broadcast(std::span<const byte> framedData) {
        for (TCPClient& client : m_clients | std::views::values) {
            static_cast<void>(client.Send(framedData));
        }
    }

    usize TCPServer::ClientCount() const noexcept {
        return m_clients.size();
    }
} // namespace Nexus::Network
