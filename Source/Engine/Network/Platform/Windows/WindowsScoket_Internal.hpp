// SPDX-License-Identifier: MIT

#pragma once

namespace Nexus::Network::Internal {
    using RawHandle = long long;

    struct RawResult {
        RawHandle value = -1;
        int platformErrno = 0;
        bool ok = false;
    };

    struct RawAddress {
        char host[64] = {}; // IPv4 string, null-terminated
        unsigned short port = 0;
    };

    struct RawReceiveResult {
        RawResult result;
        RawAddress sender;
    };

    enum class RawErrorKind {
        None,
        WouldBlock,
        ConnectionInProgress,
        ConnectionReset,
        ConnectionAborted,
        ConnectionRefused,
        NotConnected,
        AddressInUse,
        HostUnreachable,
        NetworkUnreachable,
        Timeout,
        MessageTooLarge,
        PermissionDenied,
        Unknown,
    };

    struct RawError {
        RawErrorKind kind = RawErrorKind::None;
        int platformErrno = 0;
    };

    [[nodiscard]] RawError RawTranslateLastError();
    [[nodiscard]] RawError RawTranslateErrorCode(int platformErrno);

    void RawEnsureInitialized();
    [[nodiscard]] RawHandle RawCreateSocket(bool isTcp);
    void RawCloseSocket(RawHandle handle);

    [[nodiscard]] bool RawSetNonBlocking(RawHandle handle, bool enabled);
    [[nodiscard]] bool RawSetReuseAddr(RawHandle handle);

    [[nodiscard]] bool RawBind(RawHandle handle, unsigned short port);
    [[nodiscard]] bool RawListen(RawHandle handle, int backlog);
    [[nodiscard]] RawHandle RawAccept(RawHandle handle);

    [[nodiscard]] bool RawParseIPv4(const char* host, unsigned short port, RawAddress& out);
    [[nodiscard]] RawResult RawConnect(RawHandle handle, const RawAddress& address);
    [[nodiscard]] RawResult RawCompleteConnect(RawHandle handle);

    [[nodiscard]] RawResult RawSend(RawHandle handle, const void* data, unsigned long size);
    [[nodiscard]] RawResult RawReceive(RawHandle handle, void* buffer, unsigned long size);
    [[nodiscard]] RawResult RawSendTo(RawHandle handle, const RawAddress& dest, const void* data, unsigned long size);
    [[nodiscard]] RawReceiveResult RawReceiveFrom(RawHandle handle, void* buffer, unsigned long size);
} // namespace Nexus::Network::Internal
