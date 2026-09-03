// SPDX-License-Identifier: MIT

module;

#include <Platform/DLL/Export.hpp>

export module NE.Engine.Network.Common.NetworkError;

import std;

export namespace Nexus::Network {
    enum class NetworkErrorCode {
        None,
        WouldBlock,
        ConnectionInProgress,
        ConnectionClosed,
        ConnectionReset,
        ConnectionAborted,
        ConnectionRefused,
        NotConnected,
        AddressInUse,
        HostUnreachable,
        NetworkUnreachable,
        Timeout,
        MessageTooLarge,
        InvalidAddress,
        PermissionDenied,
        InvalidOperation,
        Unknown,
    };

    struct NEXUS_API NetworkError {
        NetworkErrorCode code = NetworkErrorCode::None;
        std::string message;
        int platformErrno = 0;

        [[nodiscard]] explicit operator bool() const noexcept {
            return code != NetworkErrorCode::None;
        }
    };
} // namespace Nexus::Network
