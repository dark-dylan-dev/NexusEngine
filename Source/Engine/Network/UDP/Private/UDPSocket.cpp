// SPDX-License-Identifier: MIT

module NE.Engine.Network.UDP.UDPSocket;

import NE.Engine.Network.Common.NetworkAddress;
import NE.Engine.Network.Common.NetworkError;
import NE.Engine.Network.Platform.Socket;

import NE.Engine.Core.Types;

import std;

namespace Nexus::Network {
    bool UDPSocket::Bind(uint16 port) {
        m_socket.Close();
        m_socket = Socket(SocketType::UDP);
        if (!m_socket.IsValid()) {
            return false;
        }

        if (!m_socket.SetNonBlocking(true) || !m_socket.Bind(port)) {
            m_socket.Close();
            return false;
        }
        return true;
    }

    void UDPSocket::Close() {
        m_socket.Close();
    }

    bool UDPSocket::IsOpen() const noexcept {
        return m_socket.IsValid();
    }

    bool UDPSocket::Send(const NetworkAddress& destination, std::span<const byte> data) {
        if (!m_socket.IsValid() || data.size() > MAX_UDP_PAYLOAD) {
            return false;
        }

        std::optional<NetworkError> error;
        const auto sent = m_socket.SendTo(destination, data, error);
        return sent.has_value() && *sent == data.size();
    }

    std::optional<UDPMessage> UDPSocket::Receive(std::optional<NetworkError>& outError) {
        if (!m_socket.IsValid()) {
            outError = NetworkError{NetworkErrorCode::InvalidOperation, "UDP socket is not open", 0};
            return std::nullopt;
        }

        std::array<byte, MAX_UDP_PAYLOAD> scratch{};
        NetworkAddress sender;
        const auto received = m_socket.ReceiveFrom(scratch, sender, outError);

        if (!received) {
            return std::nullopt;
        }

        UDPMessage message;
        message.sender = std::move(sender);
        message.data.assign(scratch.begin(), scratch.begin() + static_cast<isize>(*received));
        return message;
    }
} // namespace Nexus::Network
