// SPDX-License-Identifier: MIT

module;

#include <Platform/DLL/Export.hpp>

export module NE.Engine.Network.TCP.TCPClient;

import NE.Engine.Network.Common.NetworkAddress;
import NE.Engine.Network.Common.NetworkError;
import NE.Engine.Network.Platform.Socket;
import NE.Engine.Network.TCP.StreamReassembler;

import NE.Engine.Core.Types;

import std;

export namespace Nexus::Network {
    class NEXUS_API TCPClient {
    public:
        static constexpr usize MAX_QUEUED_SEND_BYTES = 4u << 20; // 4 MiB

        TCPClient() = default;

        TCPClient(const TCPClient&) = delete;
        TCPClient& operator=(const TCPClient&) = delete;

        TCPClient(TCPClient&& other) noexcept = default;
        TCPClient& operator=(TCPClient&& other) noexcept = default;

        bool Connect(const NetworkAddress& address);

        explicit TCPClient(Socket&& acceptedSocket);

        void Disconnect() noexcept;

        [[nodiscard]] bool IsConnected() const noexcept;

        [[nodiscard]] bool IsConnecting() const noexcept;

        bool Send(std::vector<byte>&& framedData);

        bool Send(std::span<const byte> framedData);

        std::vector<std::vector<byte>> Poll();

    private:
        struct PendingSend {
            std::vector<byte> data;
            usize offset = 0;
        };

        bool FlushOutgoing();

        Socket m_socket;
        StreamReassembler m_reassembler;
        std::deque<PendingSend> m_outgoing;
        usize m_queuedSendBytes = 0;
        bool m_connected = false;
        bool m_connecting = false;
    };
} // namespace Nexus::Network
