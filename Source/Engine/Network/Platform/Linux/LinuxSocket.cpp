// SPDX-License-Identifier: MIT

module;

#include "LinuxSocket_Internal.hpp"

// clang-format off
#include <Platform/DLL/Export.hpp>

#include <cerrno>
// clang-format on

module NE.Engine.Network.Platform.Socket;

import NE.Engine.Network.Common.NetworkAddress;
import NE.Engine.Network.Common.NetworkError;

import std;

namespace Nexus::Network {
    namespace {
        using namespace Internal;

        NetworkErrorCode ToNetworkErrorCode(RawErrorKind kind) {
            switch (kind) {
                case RawErrorKind::WouldBlock:
                    return NetworkErrorCode::WouldBlock;
                case RawErrorKind::ConnectionInProgress:
                    return NetworkErrorCode::ConnectionInProgress;
                case RawErrorKind::ConnectionReset:
                    return NetworkErrorCode::ConnectionReset;
                case RawErrorKind::ConnectionAborted:
                    return NetworkErrorCode::ConnectionAborted;
                case RawErrorKind::ConnectionRefused:
                    return NetworkErrorCode::ConnectionRefused;
                case RawErrorKind::NotConnected:
                    return NetworkErrorCode::NotConnected;
                case RawErrorKind::AddressInUse:
                    return NetworkErrorCode::AddressInUse;
                case RawErrorKind::HostUnreachable:
                    return NetworkErrorCode::HostUnreachable;
                case RawErrorKind::NetworkUnreachable:
                    return NetworkErrorCode::NetworkUnreachable;
                case RawErrorKind::Timeout:
                    return NetworkErrorCode::Timeout;
                case RawErrorKind::MessageTooLarge:
                    return NetworkErrorCode::MessageTooLarge;
                case RawErrorKind::PermissionDenied:
                    return NetworkErrorCode::PermissionDenied;
                case RawErrorKind::None:
                case RawErrorKind::Unknown:
                default:
                    return NetworkErrorCode::Unknown;
            }
        }

        RawAddress ToRawAddress(const NetworkAddress& address) {
            RawAddress raw{};
            static_cast<void>(RawParseIPv4(address.Host().c_str(), address.Port(), raw));
            return raw;
        }

        NetworkAddress FromRawAddress(const RawAddress& raw) {
            return NetworkAddress(std::string(raw.host), raw.port);
        }

        void SetError(std::optional<NetworkError>& outError, NetworkErrorCode code, std::string message,
                      int platformErrno = 0) {
            outError = NetworkError{code, std::move(message), platformErrno};
        }

        void ClearError(std::optional<NetworkError>& outError) {
            outError.reset();
        }

        void SetErrorFromRaw(std::optional<NetworkError>& outError, const RawError& raw) {
            outError = NetworkError{ToNetworkErrorCode(raw.kind), std::strerror(raw.platformErrno), raw.platformErrno};
        }
    } // namespace

    NetworkError TranslateLastSocketError() {
        const RawError raw = RawTranslateErrno(errno);
        NetworkError result;
        result.code = ToNetworkErrorCode(raw.kind);
        result.platformErrno = raw.platformErrno;
        result.message = std::strerror(raw.platformErrno);
        return result;
    }

    Socket::Socket(SocketType type) : m_type(type) {
        const RawHandle handle = RawCreateSocket(type == SocketType::TCP);
        if (handle >= 0) {
            m_handle = static_cast<NativeSocketHandle>(handle);
        }
    }

    Socket::~Socket() {
        Close();
    }

    Socket::Socket(Socket&& other) noexcept
        : m_handle(std::exchange(other.m_handle, INVALID_SOCKET)),
          m_type(other.m_type) {}

    Socket& Socket::operator=(Socket&& other) noexcept {
        if (this != &other) {
            Close();
            m_handle = std::exchange(other.m_handle, INVALID_SOCKET);
            m_type = other.m_type;
        }
        return *this;
    }

    bool Socket::SetNonBlocking(bool enabled) {
        if (!IsValid()) {
            return false;
        }
        return RawSetNonBlocking(m_handle, enabled);
    }

    bool Socket::Bind(uint16 port) {
        if (!IsValid()) {
            return false;
        }

        static_cast<void>(RawSetReuseAddr(m_handle));
        return RawBind(m_handle, port);
    }

    bool Socket::Connect(const NetworkAddress& address, std::optional<NetworkError>& outError) {
        if (!IsValid() || m_type != SocketType::TCP) {
            SetError(outError, NetworkErrorCode::InvalidOperation, "Connect requires a valid TCP socket");
            return false;
        }

        const RawAddress raw = ToRawAddress(address);
        if (raw.host[0] == '\0') {
            SetError(outError, NetworkErrorCode::InvalidAddress, "invalid IPv4 address");
            return false;
        }

        const RawResult result = RawConnect(m_handle, raw);

        if (result.ok) {
            ClearError(outError);
            return true;
        }

        const RawError translated = RawTranslateErrno(result.platformErrno);
        if (translated.kind == RawErrorKind::ConnectionInProgress) {
            SetErrorFromRaw(outError, translated);
            return true;
        }

        SetErrorFromRaw(outError, translated);
        return false;
    }

    bool Socket::CompleteConnect(std::optional<NetworkError>& outError) {
        if (!IsValid() || m_type != SocketType::TCP) {
            SetError(outError, NetworkErrorCode::InvalidOperation, "CompleteConnect requires a valid TCP socket");
            return false;
        }

        const RawResult result = RawCompleteConnect(m_handle);

        if (!result.ok) {
            SetErrorFromRaw(outError, RawTranslateErrno(result.platformErrno));
            return false;
        }

        ClearError(outError);
        return true;
    }

    bool Socket::Listen(int backlog) {
        if (!IsValid() || m_type != SocketType::TCP) {
            return false;
        }
        return RawListen(m_handle, backlog);
    }

    std::optional<Socket> Socket::Accept() {
        if (!IsValid() || m_type != SocketType::TCP) {
            return std::nullopt;
        }

        const RawHandle clientHandle = RawAccept(m_handle);
        if (clientHandle < 0) {
            return std::nullopt;
        }

        Socket client;
        client.m_handle = static_cast<NativeSocketHandle>(clientHandle);
        client.m_type = SocketType::TCP;
        return client;
    }

    std::optional<usize> Socket::Send(std::span<const byte> data, std::optional<NetworkError>& outError) {
        if (!IsValid() || m_type != SocketType::TCP) {
            SetError(outError, NetworkErrorCode::InvalidOperation, "Send requires a valid TCP socket");
            return std::nullopt;
        }

        if (data.empty()) {
            ClearError(outError);
            return usize{0};
        }

        const RawResult result = RawSend(m_handle, data.data(), static_cast<unsigned long>(data.size()));
        if (!result.ok) {
            SetErrorFromRaw(outError, RawTranslateErrno(result.platformErrno));
            return std::nullopt;
        }

        ClearError(outError);
        return static_cast<usize>(result.value);
    }

    std::optional<usize> Socket::ReceiveInto(std::span<byte> buffer, std::optional<NetworkError>& outError) {
        if (!IsValid() || m_type != SocketType::TCP) {
            SetError(outError, NetworkErrorCode::InvalidOperation, "Receive requires a valid TCP socket");
            return std::nullopt;
        }

        if (buffer.empty()) {
            ClearError(outError);
            return usize{0};
        }

        const RawResult result = RawReceive(m_handle, buffer.data(), static_cast<unsigned long>(buffer.size()));

        if (result.ok && result.value == 0) {
            SetError(outError, NetworkErrorCode::ConnectionClosed, "peer closed");
            return std::nullopt;
        }

        if (!result.ok) {
            SetErrorFromRaw(outError, RawTranslateErrno(result.platformErrno));
            return std::nullopt;
        }

        ClearError(outError);
        return static_cast<usize>(result.value);
    }

    std::optional<usize> Socket::SendTo(const NetworkAddress& dest, std::span<const byte> data,
                                        std::optional<NetworkError>& outError) {
        if (!IsValid() || m_type != SocketType::UDP) {
            SetError(outError, NetworkErrorCode::InvalidOperation, "SendTo requires a valid UDP socket");
            return std::nullopt;
        }

        const RawAddress raw = ToRawAddress(dest);
        if (raw.host[0] == '\0') {
            SetError(outError, NetworkErrorCode::InvalidAddress, "invalid IPv4 address");
            return std::nullopt;
        }

        const RawResult result = RawSendTo(m_handle, raw, data.data(), static_cast<unsigned long>(data.size()));
        if (!result.ok) {
            SetErrorFromRaw(outError, RawTranslateErrno(result.platformErrno));
            return std::nullopt;
        }

        ClearError(outError);
        return static_cast<usize>(result.value);
    }

    std::optional<usize> Socket::ReceiveFrom(std::span<byte> buffer, NetworkAddress& outSender,
                                             std::optional<NetworkError>& outError) {
        if (!IsValid() || m_type != SocketType::UDP) {
            SetError(outError, NetworkErrorCode::InvalidOperation, "ReceiveFrom requires a valid UDP socket");
            return std::nullopt;
        }

        const RawReceiveResult result =
            RawReceiveFrom(m_handle, buffer.data(), static_cast<unsigned long>(buffer.size()));

        if (!result.result.ok) {
            SetErrorFromRaw(outError, RawTranslateErrno(result.result.platformErrno));
            return std::nullopt;
        }

        outSender = FromRawAddress(result.sender);
        ClearError(outError);
        return static_cast<usize>(result.result.value);
    }

    void Socket::Close() {
        if (!IsValid()) {
            return;
        }

        RawCloseSocket(m_handle);
        m_handle = INVALID_SOCKET;
    }
} // namespace Nexus::Network
