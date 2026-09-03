// SPDX-License-Identifier: MIT

#include <gtest/gtest.h>

import NE.Engine.Network.Common.NetworkError;

import std;

using Nexus::Network::NetworkError;
using Nexus::Network::NetworkErrorCode;

TEST(NetworkErrorTest, DefaultConstructedHasNoneCode) {
    const NetworkError error;

    EXPECT_EQ(error.code, NetworkErrorCode::None);
    EXPECT_TRUE(error.message.empty());
    EXPECT_EQ(error.platformErrno, 0);
}

TEST(NetworkErrorTest, DefaultConstructedIsFalsy) {
    const NetworkError error;

    EXPECT_FALSE(static_cast<bool>(error));
}

TEST(NetworkErrorTest, NonNoneCodeIsTruthy) {
    NetworkError error;
    error.code = NetworkErrorCode::Timeout;

    EXPECT_TRUE(static_cast<bool>(error));
}

TEST(NetworkErrorTest, StoresMessageAndPlatformErrno) {
    const NetworkError error{NetworkErrorCode::ConnectionReset, "connection reset", 104};

    EXPECT_EQ(error.code, NetworkErrorCode::ConnectionReset);
    EXPECT_EQ(error.message, "connection reset");
    EXPECT_EQ(error.platformErrno, 104);
}

TEST(NetworkErrorTest, EveryNonNoneCodeIsTruthy) {
    constexpr NetworkErrorCode codes[] = {
        NetworkErrorCode::WouldBlock,        NetworkErrorCode::ConnectionInProgress,
        NetworkErrorCode::ConnectionClosed,  NetworkErrorCode::ConnectionReset,
        NetworkErrorCode::ConnectionAborted, NetworkErrorCode::ConnectionRefused,
        NetworkErrorCode::NotConnected,      NetworkErrorCode::AddressInUse,
        NetworkErrorCode::HostUnreachable,   NetworkErrorCode::NetworkUnreachable,
        NetworkErrorCode::Timeout,           NetworkErrorCode::MessageTooLarge,
        NetworkErrorCode::InvalidAddress,    NetworkErrorCode::PermissionDenied,
        NetworkErrorCode::InvalidOperation,  NetworkErrorCode::Unknown,
    };

    for (const auto code : codes) {
        NetworkError error;
        error.code = code;
        EXPECT_TRUE(static_cast<bool>(error)) << "code index " << static_cast<int>(code);
    }
}
