// SPDX-License-Identifier: MIT

module;

#include <Platform/DLL/Export.hpp>

export module NE.Engine.Network.Platform.Socket;

import NE.Engine.Network.Common.NetworkAddress;
import NE.Engine.Network.Common.NetworkError;

import NE.Engine.Core.Types;

import std;

export namespace Nexus::Network {
    enum class SocketType {
        TCP,
        UDP,
    };

    using NativeSocketHandle = uptr;
    inline constexpr NativeSocketHandle INVALID_SOCKET = ~0; // not -1 because unsigned

    [[nodiscard]] NEXUS_API NetworkError TranslateLastSocketError();

    class NEXUS_API Socket {
    public:
        Socket() = default;
        explicit Socket(SocketType type);
        ~Socket();

        Socket(const Socket&) = delete;
        Socket& operator=(const Socket&) = delete;

        Socket(Socket&& other) noexcept;
        Socket& operator=(Socket&& other) noexcept;

        [[nodiscard]] bool IsValid() const noexcept {
            return m_handle != INVALID_SOCKET;
        }

        bool SetNonBlocking(bool enabled);
        bool Bind(uint16 port);

        bool Connect(const NetworkAddress& address, std::optional<NetworkError>& outError);
        [[nodiscard]] bool CompleteConnect(std::optional<NetworkError>& outError);
        bool Listen(int backlog = 16);
        [[nodiscard]] std::optional<Socket> Accept();

        std::optional<usize> Send(std::span<const byte> data, std::optional<NetworkError>& outError);

        std::optional<usize> ReceiveInto(std::span<byte> buffer, std::optional<NetworkError>& outError);

        std::optional<usize> SendTo(const NetworkAddress& dest, std::span<const byte> data,
                                    std::optional<NetworkError>& outError);

        std::optional<usize> ReceiveFrom(std::span<byte> buffer, NetworkAddress& outSender,
                                         std::optional<NetworkError>& outError);

        void Close();

    private:
        NativeSocketHandle m_handle = INVALID_SOCKET;
        SocketType m_type = SocketType::TCP;
    };

} // namespace Nexus::Network
