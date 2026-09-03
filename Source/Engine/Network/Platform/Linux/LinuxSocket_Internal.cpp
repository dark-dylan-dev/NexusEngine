// SPDX-License-Identifier: MIT

#include "LinuxSocket_Internal.hpp"

// clang-format off
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>
// clang-format on

namespace Nexus::Network::Internal {
    namespace {
        void ToSockAddr(const RawAddress& addr, sockaddr_in& out) {
            out = {};
            out.sin_family = AF_INET;
            out.sin_port = htons(addr.port);
            ::inet_pton(AF_INET, addr.host, &out.sin_addr);
        }

        void FromSockAddr(const sockaddr_in& sa, RawAddress& out) {
            out = {};
            ::inet_ntop(AF_INET, &sa.sin_addr, out.host, sizeof(out.host));
            out.port = ntohs(sa.sin_port);
        }

        RawResult MakeResult(RawHandle value) {
            RawResult result;
            result.value = value;
            result.ok = value >= 0;
            result.platformErrno = result.ok ? 0 : errno;
            return result;
        }
    } // namespace

    RawError RawTranslateErrno(int err) {
        RawError result;
        result.platformErrno = err;

        switch (err) {
            case EWOULDBLOCK: // == EAGAIN
                result.kind = RawErrorKind::WouldBlock;
                break;
            case EINPROGRESS:
            case EALREADY:
                result.kind = RawErrorKind::ConnectionInProgress;
                break;
            case ECONNRESET:
                result.kind = RawErrorKind::ConnectionReset;
                break;
            case ECONNABORTED:
                result.kind = RawErrorKind::ConnectionAborted;
                break;
            case ECONNREFUSED:
                result.kind = RawErrorKind::ConnectionRefused;
                break;
            case ENOTCONN:
                result.kind = RawErrorKind::NotConnected;
                break;
            case EADDRINUSE:
                result.kind = RawErrorKind::AddressInUse;
                break;
            case EHOSTUNREACH:
                result.kind = RawErrorKind::HostUnreachable;
                break;
            case ENETUNREACH:
                result.kind = RawErrorKind::NetworkUnreachable;
                break;
            case ETIMEDOUT:
                result.kind = RawErrorKind::Timeout;
                break;
            case EMSGSIZE:
                result.kind = RawErrorKind::MessageTooLarge;
                break;
            case EACCES:
                result.kind = RawErrorKind::PermissionDenied;
                break;
            default:
                result.kind = RawErrorKind::Unknown;
                break;
        }
        return result;
    }

    RawHandle RawCreateSocket(bool isTcp) {
        const int stype = isTcp ? SOCK_STREAM : SOCK_DGRAM;
        return ::socket(AF_INET, stype, 0);
    }

    void RawCloseSocket(RawHandle handle) {
        ::close(static_cast<int>(handle));
    }

    bool RawSetNonBlocking(RawHandle handle, bool enabled) {
        const int fd = static_cast<int>(handle);
        const int flags = ::fcntl(fd, F_GETFL, 0);
        if (flags == -1) {
            return false;
        }

        const int newFlags = enabled ? (flags | O_NONBLOCK) : (flags & ~O_NONBLOCK);
        return ::fcntl(fd, F_SETFL, newFlags) != -1;
    }

    bool RawSetReuseAddr(RawHandle handle) {
        const int reuse = 1;
        return ::setsockopt(static_cast<int>(handle), SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == 0;
    }

    bool RawBind(RawHandle handle, unsigned short port) {
        sockaddr_in sa{};
        sa.sin_family = AF_INET;
        sa.sin_addr.s_addr = htonl(INADDR_ANY);
        sa.sin_port = htons(port);

        return ::bind(static_cast<int>(handle), reinterpret_cast<sockaddr*>(&sa), sizeof(sa)) == 0;
    }

    bool RawListen(RawHandle handle, int backlog) {
        return ::listen(static_cast<int>(handle), backlog) == 0;
    }

    RawHandle RawAccept(RawHandle handle) {
        sockaddr_in sa{};
        socklen_t len = sizeof(sa);
        return ::accept(static_cast<int>(handle), reinterpret_cast<sockaddr*>(&sa), &len);
    }

    bool RawParseIPv4(const char* host, unsigned short port, RawAddress& out) {
        sockaddr_in sa{};
        sa.sin_family = AF_INET;
        sa.sin_port = htons(port);
        if (::inet_pton(AF_INET, host, &sa.sin_addr) != 1) {
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

        const int result = ::connect(static_cast<int>(handle), reinterpret_cast<const sockaddr*>(&sa), sizeof(sa));
        return MakeResult(result);
    }

    RawResult RawCompleteConnect(RawHandle handle) {
        const int fd = static_cast<int>(handle);

        fd_set writeSet;
        fd_set errorSet;
        FD_ZERO(&writeSet);
        FD_ZERO(&errorSet);
        FD_SET(fd, &writeSet);
        FD_SET(fd, &errorSet);

        timeval timeout{};
        const int ready = ::select(fd + 1, nullptr, &writeSet, &errorSet, &timeout);
        if (ready < 0) {
            return MakeResult(-1);
        }

        if (ready == 0) {
            RawResult pending;
            pending.value = 0;
            pending.ok = false;
            pending.platformErrno = EINPROGRESS;
            return pending;
        }

        int socketError = 0;
        socklen_t errorSize = sizeof(socketError);
        if (::getsockopt(fd, SOL_SOCKET, SO_ERROR, &socketError, &errorSize) != 0) {
            return MakeResult(-1);
        }

        if (socketError != 0) {
            RawResult failed;
            failed.value = -1;
            failed.ok = false;
            failed.platformErrno = socketError;
            return failed;
        }

        return MakeResult(0);
    }

    RawResult RawSend(RawHandle handle, const void* data, unsigned long size) {
        const ssize_t n = ::send(static_cast<int>(handle), data, size, 0);
        return MakeResult(static_cast<RawHandle>(n));
    }

    RawResult RawReceive(RawHandle handle, void* buffer, unsigned long size) {
        const ssize_t n = ::recv(static_cast<int>(handle), buffer, size, 0);
        return MakeResult(static_cast<RawHandle>(n));
    }

    RawResult RawSendTo(RawHandle handle, const RawAddress& dest, const void* data, unsigned long size) {
        sockaddr_in sa{};
        ToSockAddr(dest, sa);

        const ssize_t n =
            ::sendto(static_cast<int>(handle), data, size, 0, reinterpret_cast<const sockaddr*>(&sa), sizeof(sa));
        return MakeResult(static_cast<RawHandle>(n));
    }

    RawReceiveResult RawReceiveFrom(RawHandle handle, void* buffer, unsigned long size) {
        sockaddr_in sa{};
        socklen_t len = sizeof(sa);
        const ssize_t n = ::recvfrom(static_cast<int>(handle), buffer, size, 0, reinterpret_cast<sockaddr*>(&sa), &len);

        RawReceiveResult result;
        result.result = MakeResult(static_cast<RawHandle>(n));
        if (result.result.ok) {
            FromSockAddr(sa, result.sender);
        }
        return result;
    }
} // namespace Nexus::Network::Internal
