// SPDX-License-Identifier: MIT

module;

#include <Platform/DLL/Export.hpp>

export module NE.Engine.Network.UDP.UDPSocket;

import NE.Engine.Network.Common.NetworkAddress;
import NE.Engine.Network.Common.NetworkError;
import NE.Engine.Network.Platform.Socket;

import NE.Engine.Core.Types;

import std;

export namespace Nexus::Network {
    inline constexpr usize MAX_UDP_PAYLOAD = 65507;

    struct NEXUS_API UDPMessage {
        NetworkAddress sender;
        std::vector<byte> data;
    };

    class NEXUS_API UDPSocket {
    public:
        UDPSocket() = default;

        UDPSocket(const UDPSocket&) = delete;
        UDPSocket& operator=(const UDPSocket&) = delete;
        UDPSocket(UDPSocket&&) noexcept = default;
        UDPSocket& operator=(UDPSocket&&) noexcept = default;

        bool Bind(uint16 port);

        void Close();

        [[nodiscard]] bool IsOpen() const noexcept;

        bool Send(const NetworkAddress& destination, std::span<const byte> data);

        /// Returns one datagram. Call repeatedly until it returns nullopt.
        [[nodiscard]] std::optional<UDPMessage> Receive(std::optional<NetworkError>& outError);

    private:
        Socket m_socket;
    };
} // namespace Nexus::Network
