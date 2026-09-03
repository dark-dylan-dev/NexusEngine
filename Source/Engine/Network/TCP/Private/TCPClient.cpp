// SPDX-License-Identifier: MIT

module NE.Engine.Network.TCP.TCPClient;

import NE.Engine.Network.Common.NetworkAddress;
import NE.Engine.Network.Common.NetworkError;
import NE.Engine.Network.Platform.Socket;
import NE.Engine.Network.TCP.StreamReassembler;

import NE.Engine.Core.Types;

import std;

namespace Nexus::Network {
    bool TCPClient::Connect(const NetworkAddress& address) {
        Disconnect();

        m_socket = Socket(SocketType::TCP);
        if (!m_socket.IsValid()) {
            return false;
        }

        if (!m_socket.SetNonBlocking(true)) {
            m_socket.Close();
            return false;
        }

        std::optional<NetworkError> error;
        if (!m_socket.Connect(address, error)) {
            m_socket.Close();
            return false;
        }

        if (error && error->code == NetworkErrorCode::ConnectionInProgress) {
            m_connecting = true;
            m_connected = false;
        } else {
            m_connecting = false;
            m_connected = true;
        }

        return true;
    }

    TCPClient::TCPClient(Socket&& acceptedSocket)
        : m_socket(std::move(acceptedSocket)),
          m_connected(m_socket.IsValid()) {
        m_connecting = false;

        if (m_connected && !m_socket.SetNonBlocking(true)) {
            Disconnect();
        }
    }

    void TCPClient::Disconnect() noexcept {
        m_socket.Close();
        m_connecting = false;
        m_connected = false;
        m_outgoing.clear();
        m_queuedSendBytes = 0;
        m_reassembler.Reset();
    }

    bool TCPClient::IsConnected() const noexcept {
        return m_connected;
    }

    bool TCPClient::IsConnecting() const noexcept {
        return m_connecting;
    }

    bool TCPClient::Send(std::vector<byte>&& framedData) {
        if ((!m_connected && !m_connecting) || framedData.empty()) {
            return false;
        }

        if (framedData.size() > MAX_QUEUED_SEND_BYTES - m_queuedSendBytes) {
            return false;
        }

        m_queuedSendBytes += framedData.size();
        m_outgoing.emplace_back(std::move(framedData), 0);

        if (m_connected) {
            FlushOutgoing();
        }

        return m_connected || m_connecting;
    }

    bool TCPClient::Send(std::span<const byte> framedData) {
        if (framedData.empty()) {
            return false;
        }

        std::vector<byte> copy(framedData.begin(), framedData.end());
        return Send(std::move(copy));
    }

    std::vector<std::vector<byte>> TCPClient::Poll() {
        std::vector<std::vector<byte>> frames;

        if (!m_socket.IsValid()) {
            return frames;
        }

        if (m_connecting) {
            std::optional<NetworkError> error;

            if (!m_socket.CompleteConnect(error)) {
                if (error && error->code == NetworkErrorCode::ConnectionInProgress) {
                    return frames;
                }

                Disconnect();
                return frames;
            }

            m_connecting = false;
            m_connected = true;
        }

        if (!m_connected) {
            return frames;
        }

        FlushOutgoing();
        if (!m_connected) {
            return frames;
        }

        std::array<byte, 16 * 1024> scratch{};

        while (true) {
            std::optional<NetworkError> error;
            const auto received = m_socket.ReceiveInto(scratch, error);

            if (!received) {
                if (error && error->code == NetworkErrorCode::WouldBlock) {
                    break;
                }

                Disconnect();
                break;
            }

            if (*received == 0) {
                Disconnect();
                break;
            }

            m_reassembler.Feed(std::span<const byte>(scratch.data(), *received));

            if (m_reassembler.HasInvalidData()) {
                Disconnect();
                break;
            }
        }

        while (auto frame = m_reassembler.TryExtract()) {
            frames.push_back(std::move(*frame));
        }

        return frames;
    }

    bool TCPClient::FlushOutgoing() {
        if (!m_connected || !m_socket.IsValid()) {
            return true;
        }

        while (!m_outgoing.empty()) {
            PendingSend& pending = m_outgoing.front();

            if (pending.offset >= pending.data.size()) {
                m_outgoing.pop_front();
                continue;
            }

            const std::span<const byte> remaining(pending.data.data() + pending.offset,
                                                  pending.data.size() - pending.offset);

            std::optional<NetworkError> error;
            const auto sent = m_socket.Send(remaining, error);

            if (!sent) {
                if (error && error->code == NetworkErrorCode::WouldBlock) {
                    return true;
                }

                Disconnect();
                return false;
            }

            if (*sent == 0) {
                Disconnect();
                return false;
            }

            pending.offset += *sent;
            m_queuedSendBytes -= *sent;

            if (pending.offset == pending.data.size()) {
                m_outgoing.pop_front();
            }
        }

        return true;
    }
} // namespace Nexus::Network
