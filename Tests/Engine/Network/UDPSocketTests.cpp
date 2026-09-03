// SPDX-License-Identifier: MIT

#include <gtest/gtest.h>

import NE.Engine.Network.Common.NetworkAddress;
import NE.Engine.Network.Common.NetworkError;
import NE.Engine.Network.UDP.UDPSocket;

import NE.Engine.Core.Types;

import std;

using namespace Nexus::Network;

namespace {
    template <typename Predicate>
    bool WaitUntil(Predicate predicate, std::chrono::milliseconds timeout = std::chrono::milliseconds(2000)) {
        const auto deadline = std::chrono::steady_clock::now() + timeout;
        while (std::chrono::steady_clock::now() < deadline) {
            if (predicate()) {
                return true;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
        return predicate();
    }

    Nexus::uint16 NextTestPort() {
        static std::atomic<Nexus::uint16> counter{54000};
        return counter.fetch_add(1);
    }

    class UDPSocketTest : public ::testing::Test {};
} // namespace

TEST_F(UDPSocketTest, DefaultConstructedIsNotOpen) {
    const UDPSocket socket;
    EXPECT_FALSE(socket.IsOpen());
}

TEST_F(UDPSocketTest, BindSucceedsAndOpensSocket) {
    UDPSocket socket;
    EXPECT_TRUE(socket.Bind(NextTestPort()));
    EXPECT_TRUE(socket.IsOpen());
}

TEST_F(UDPSocketTest, BindToEphemeralPortSucceeds) {
    UDPSocket socket;
    EXPECT_TRUE(socket.Bind(0));
    EXPECT_TRUE(socket.IsOpen());
}

TEST_F(UDPSocketTest, CloseInvalidatesSocket) {
    UDPSocket socket;
    ASSERT_TRUE(socket.Bind(NextTestPort()));

    socket.Close();
    EXPECT_FALSE(socket.IsOpen());
}

TEST_F(UDPSocketTest, SendOnUnboundSocketFails) {
    UDPSocket socket;
    const std::array<Nexus::byte, 4> data{};

    EXPECT_FALSE(socket.Send(NetworkAddress("127.0.0.1", 1234), data));
}

TEST_F(UDPSocketTest, ReceiveOnUnboundSocketFailsWithInvalidOperation) {
    UDPSocket socket;
    std::optional<NetworkError> error;

    const auto message = socket.Receive(error);

    EXPECT_FALSE(message.has_value());
    ASSERT_TRUE(error.has_value());
    EXPECT_EQ(error->code, NetworkErrorCode::InvalidOperation);
}

TEST_F(UDPSocketTest, SendThenReceiveRoundTripsPayload) {
    const Nexus::uint16 serverPort = NextTestPort();

    UDPSocket server;
    ASSERT_TRUE(server.Bind(serverPort));

    UDPSocket client;
    ASSERT_TRUE(client.Bind(0));

    const std::array<Nexus::byte, 4> payload{Nexus::byte{'p'}, Nexus::byte{'i'}, Nexus::byte{'n'}, Nexus::byte{'g'}};
    EXPECT_TRUE(client.Send(NetworkAddress("127.0.0.1", serverPort), payload));

    std::optional<UDPMessage> message;
    const bool got = WaitUntil([&] {
        std::optional<NetworkError> error;
        message = server.Receive(error);
        return message.has_value();
    });

    ASSERT_TRUE(got);
    ASSERT_EQ(message->data.size(), payload.size());
    EXPECT_TRUE(std::equal(payload.begin(), payload.end(), message->data.begin()));
    EXPECT_EQ(message->sender.Host(), "127.0.0.1");
}

TEST_F(UDPSocketTest, ReceiveWithNoDatagramReturnsNulloptWithoutError) {
    UDPSocket socket;
    ASSERT_TRUE(socket.Bind(NextTestPort()));

    std::optional<NetworkError> error;
    const auto message = socket.Receive(error);

    EXPECT_FALSE(message.has_value());
    if (error.has_value()) {
        EXPECT_EQ(error->code, NetworkErrorCode::WouldBlock);
    }
}

TEST_F(UDPSocketTest, SendPayloadLargerThanMaxUdpPayloadFails) {
    UDPSocket socket;
    ASSERT_TRUE(socket.Bind(NextTestPort()));

    const std::vector<Nexus::byte> oversized(MAX_UDP_PAYLOAD + 1, Nexus::byte{0});
    EXPECT_FALSE(socket.Send(NetworkAddress("127.0.0.1", NextTestPort()), oversized));
}

TEST_F(UDPSocketTest, MultipleDatagramsAreReceivedInSeparateCalls) {
    const Nexus::uint16 serverPort = NextTestPort();

    UDPSocket server;
    ASSERT_TRUE(server.Bind(serverPort));

    UDPSocket client;
    ASSERT_TRUE(client.Bind(0));

    const std::array<Nexus::byte, 1> first{Nexus::byte{1}};
    const std::array<Nexus::byte, 1> second{Nexus::byte{2}};
    EXPECT_TRUE(client.Send(NetworkAddress("127.0.0.1", serverPort), first));
    EXPECT_TRUE(client.Send(NetworkAddress("127.0.0.1", serverPort), second));

    std::vector<Nexus::byte> receivedValues;
    WaitUntil([&] {
        std::optional<NetworkError> error;
        if (auto message = server.Receive(error)) {
            receivedValues.insert(receivedValues.end(), message->data.begin(), message->data.end());
        }
        return receivedValues.size() >= 2;
    });

    ASSERT_EQ(receivedValues.size(), 2u);
    EXPECT_EQ(static_cast<int>(receivedValues[0]), 1);
    EXPECT_EQ(static_cast<int>(receivedValues[1]), 2);
}

TEST_F(UDPSocketTest, IsMoveOnly) {
    EXPECT_FALSE(std::is_copy_constructible_v<UDPSocket>);
    EXPECT_FALSE(std::is_copy_assignable_v<UDPSocket>);
    EXPECT_TRUE(std::is_move_constructible_v<UDPSocket>);
    EXPECT_TRUE(std::is_move_assignable_v<UDPSocket>);
}

TEST_F(UDPSocketTest, RebindClosesPreviousSocket) {
    UDPSocket socket;
    ASSERT_TRUE(socket.Bind(NextTestPort()));
    ASSERT_TRUE(socket.IsOpen());

    ASSERT_TRUE(socket.Bind(NextTestPort()));
    EXPECT_TRUE(socket.IsOpen());
}
