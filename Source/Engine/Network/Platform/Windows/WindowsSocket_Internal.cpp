// SPDX-License-Identifier: MIT

#include "WindowsScoket_Internal.hpp"

// clang-format off
#if !defined(NOMINMAX)
#    define NOMINMAX
#endif
#if !defined(WIN32_LEAN_AND_MEAN)
#    define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <ws2tcpip.h>

#include <cstring>
// clang-format on

namespace Nexus::Network::Internal {
    namespace {
        void ToSockAddr(const RawAddress& addr, sockaddr_in& out) {
            out = {};
            out.sin_family = AF_INET;
            out.sin_port = htons(addr.port);
            InetPtonA(AF_INET, addr.host, &out.sin_addr);
        }

        void FromSockAddr(const sockaddr_in& sa, RawAddress& out) {
            out = {};
            InetNtopA(AF_INET, const_cast<IN_ADDR*>(&sa.sin_addr), out.host, sizeof(out.host));
            out.port = ntohs(sa.sin_port);
        }

        RawResult MakeResult(RawHandle value) {
            RawResult result;
            result.value = value;
            result.ok = value != SOCKET_ERROR;
            result.platformErrno = result.ok ? 0 : WSAGetLastError();
            return result;
        }
    } // namespace

    void RawEnsureInitialized() {
        static const bool initialized = [] {
            WSADATA data{};
            return WSAStartup(MAKEWORD(2, 2), &data) == 0;
        }();
        static_cast<void>(initialized);
    }

    RawError RawTranslateErrorCode(int err) {
        RawError result;
        result.platformErrno = err;

        switch (err) {
            case WSAEWOULDBLOCK:
                result.kind = RawErrorKind::WouldBlock;
                break;
            case WSAEINPROGRESS:
            case WSAEALREADY:
                result.kind = RawErrorKind::ConnectionInProgress;
                break;
            case WSAECONNRESET:
                result.kind = RawErrorKind::ConnectionReset;
                break;
            case WSAECONNABORTED:
                result.kind = RawErrorKind::ConnectionAborted;
                break;
            case WSAECONNREFUSED:
                result.kind = RawErrorKind::ConnectionRefused;
                break;
            case WSAENOTCONN:
                result.kind = RawErrorKind::NotConnected;
                break;
            case WSAEADDRINUSE:
                result.kind = RawErrorKind::AddressInUse;
                break;
            case WSAEHOSTUNREACH:
                result.kind = RawErrorKind::HostUnreachable;
                break;
            case WSAENETUNREACH:
                result.kind = RawErrorKind::NetworkUnreachable;
                break;
            case WSAETIMEDOUT:
                result.kind = RawErrorKind::Timeout;
                break;
            case WSAEMSGSIZE:
                result.kind = RawErrorKind::MessageTooLarge;
                break;
            case WSAEACCES:
                result.kind = RawErrorKind::PermissionDenied;
                break;
            default:
                result.kind = RawErrorKind::Unknown;
                break;
        }
        return result;
    }

    RawError RawTranslateLastError() {
        return RawTranslateErrorCode(WSAGetLastError());
    }

    RawHandle RawCreateSocket(bool isTcp) {
        RawEnsureInitialized();

        const int socketType = isTcp ? SOCK_STREAM : SOCK_DGRAM;
        const int protocol = isTcp ? IPPROTO_TCP : IPPROTO_UDP;
        const SOCKET handle = ::socket(AF_INET, socketType, protocol);
        return handle == INVALID_SOCKET ? -1 : static_cast<RawHandle>(handle);
    }

    void RawCloseSocket(RawHandle handle) {
        ::closesocket(static_cast<SOCKET>(handle));
    }

    bool RawSetNonBlocking(RawHandle handle, bool enabled) {
        u_long mode = enabled ? 1UL : 0UL;
        return ioctlsocket(static_cast<SOCKET>(handle), FIONBIO, &mode) == 0;
    }

    bool RawSetReuseAddr(RawHandle handle) {
        const BOOL reuse = TRUE;
        return ::setsockopt(static_cast<SOCKET>(handle), SOL_SOCKET, SO_REUSEADDR,
                            reinterpret_cast<const char*>(&reuse), sizeof(reuse)) == 0;
    }

    bool RawBind(RawHandle handle, unsigned short port) {
        sockaddr_in sa{};
        sa.sin_family = AF_INET;
        sa.sin_addr.s_addr = htonl(INADDR_ANY);
        sa.sin_port = htons(port);

        return ::bind(static_cast<SOCKET>(handle), reinterpret_cast<const sockaddr*>(&sa), sizeof(sa)) == 0;
    }

    bool RawListen(RawHandle handle, int backlog) {
        return ::listen(static_cast<SOCKET>(handle), backlog) == 0;
    }

    RawHandle RawAccept(RawHandle handle) {
        sockaddr_in sa{};
        int len = sizeof(sa);
        const SOCKET clientSocket = ::accept(static_cast<SOCKET>(handle), reinterpret_cast<sockaddr*>(&sa), &len);
        return clientSocket == INVALID_SOCKET ? -1 : static_cast<RawHandle>(clientSocket);
    }

    bool RawParseIPv4(const char* host, unsigned short port, RawAddress& out) {
        sockaddr_in sa{};
        if (InetPtonA(AF_INET, host, &sa.sin_addr) != 1) {
            return false;
        }

        out = {};
        std::strncpy(out.host, host, sizeof(out.host) - 1);
        out.port = port;
        return true;
    }

    RawResult RawConnect(RawHandle handle, const RawAddress& address) {
        sockaddr_in sa{};
        ToSockAddr(address, sa);

        const int result = ::connect(static_cast<SOCKET>(handle), reinterpret_cast<const sockaddr*>(&sa), sizeof(sa));
        return MakeResult(result == 0 ? 0 : SOCKET_ERROR);
    }

    RawResult RawCompleteConnect(RawHandle handle) {
        fd_set writeSet{};
        fd_set errorSet{};
        FD_ZERO(&writeSet);
        FD_ZERO(&errorSet);

        const SOCKET socketHandle = static_cast<SOCKET>(handle);
        FD_SET(socketHandle, &writeSet);
        FD_SET(socketHandle, &errorSet);

        timeval timeout{};
        const int ready = ::select(0, nullptr, &writeSet, &errorSet, &timeout);
        if (ready == SOCKET_ERROR) {
            return MakeResult(SOCKET_ERROR);
        }

        if (ready == 0) {
            RawResult pending;
            pending.value = 0;
            pending.ok = false;
            pending.platformErrno = WSAEINPROGRESS;
            return pending;
        }

        int socketError = 0;
        int errorSize = sizeof(socketError);
        if (getsockopt(socketHandle, SOL_SOCKET, SO_ERROR, reinterpret_cast<char*>(&socketError), &errorSize) != 0) {
            return MakeResult(SOCKET_ERROR);
        }

        if (socketError != 0) {
            WSASetLastError(socketError);
            return MakeResult(SOCKET_ERROR);
        }

        return MakeResult(0);
    }

    RawResult RawSend(RawHandle handle, const void* data, unsigned long size) {
        static constexpr auto MAXCONN = static_cast<unsigned long>(SOMAXCONN);
        const int clamped = static_cast<int>(size > MAXCONN ? MAXCONN : size);
        const int n = ::send(static_cast<SOCKET>(handle), static_cast<const char*>(data), clamped, 0);
        return MakeResult(n);
    }

    RawResult RawReceive(RawHandle handle, void* buffer, unsigned long size) {
        static constexpr auto MAXCONN = static_cast<unsigned long>(SOMAXCONN);
        const int clamped = static_cast<int>(size > MAXCONN ? MAXCONN : size);
        const int n = ::recv(static_cast<SOCKET>(handle), static_cast<char*>(buffer), clamped, 0);
        return MakeResult(n);
    }

    RawResult RawSendTo(RawHandle handle, const RawAddress& dest, const void* data, unsigned long size) {
        sockaddr_in sa{};
        ToSockAddr(dest, sa);

        static constexpr auto MAXCONN = static_cast<unsigned long>(SOMAXCONN);
        const int clamped = static_cast<int>(size > MAXCONN ? MAXCONN : size);
        const int n = ::sendto(static_cast<SOCKET>(handle), static_cast<const char*>(data), clamped, 0,
                               reinterpret_cast<const sockaddr*>(&sa), sizeof(sa));
        return MakeResult(n);
    }

    RawReceiveResult RawReceiveFrom(RawHandle handle, void* buffer, unsigned long size) {
        sockaddr_in sa{};
        int len = sizeof(sa);

        static constexpr auto MAXCONN = static_cast<unsigned long>(SOMAXCONN);
        const int clamped = static_cast<int>(size > MAXCONN ? MAXCONN : size);
        const int n = ::recvfrom(static_cast<SOCKET>(handle), static_cast<char*>(buffer), clamped, 0,
                                 reinterpret_cast<sockaddr*>(&sa), &len);

        RawReceiveResult result;
        result.result = MakeResult(n);
        if (result.result.ok) {
            FromSockAddr(sa, result.sender);
        }
        return result;
    }
} // namespace Nexus::Network::Internal
