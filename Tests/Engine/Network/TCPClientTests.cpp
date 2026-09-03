// SPDX-License-Identifier: MIT

#include <gtest/gtest.h>

import NE.Engine.Network.Common.NetworkAddress;
import NE.Engine.Network.Common.NetworkError;
import NE.Engine.Network.Packet;
import NE.Engine.Network.PacketWriter;
import NE.Engine.Network.Platform.Socket;
import NE.Engine.Network.TCP.TCPClient;

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
        static std::atomic<Nexus::uint16> counter{52000};
        return counter.fetch_add(1);
    }

    struct RawListener {
        Socket listener;
        Nexus::uint16 port = 0;

        static std::optional<RawListener> Create() {
            RawListener result;
            result.port = NextTestPort();
            result.listener = Socket(SocketType::TCP);
            if (!result.listener.IsValid() || !result.listener.SetNonBlocking(true) ||
                !result.listener.Bind(result.port) || !result.listener.Listen()) {
                return std::nullopt;
            }
            return result;
        }

        std::optional<Socket> WaitForAccept() {
            std::optional<Socket> accepted;
            WaitUntil([&] {
                accepted = listener.Accept();
                return accepted.has_value();
            });
            const bool nonBlocking = accepted->SetNonBlocking(true);
            return accepted;
        }
    };

    std::vector<Nexus::byte> BuildFrame(std::string_view text) {
        PacketWriter writer(PacketType::ChatMessage, 1);
        writer.WriteString(text);
        return writer.Build();
    }

    class TCPClientTest : public ::testing::Test {};
} // namespace

TEST_F(TCPClientTest, DefaultConstructedIsNotConnected) {
    const TCPClient client;
    EXPECT_FALSE(client.IsConnected());
    EXPECT_FALSE(client.IsConnecting());
}

TEST_F(TCPClientTest, ConnectToClosedPortFailsOrEndsUpDisconnected) {
    TCPClient client;
    const Nexus::uint16 port = NextTestPort(); // nothing listening here

    const bool started = client.Connect(NetworkAddress("127.0.0.1", port));
    if (!started) {
        EXPECT_FALSE(client.IsConnected());
        return;
    }

    WaitUntil([&] {
        client.Poll();
        return !client.IsConnecting();
    });

    EXPECT_FALSE(client.IsConnected());
}

TEST_F(TCPClientTest, ConnectThenPollBecomesConnected) {
    auto listener = RawListener::Create();
    ASSERT_TRUE(listener.has_value());

    TCPClient client;
    ASSERT_TRUE(client.Connect(NetworkAddress("127.0.0.1", listener->port)));

    auto accepted = listener->WaitForAccept();
    ASSERT_TRUE(accepted.has_value());

    const bool connected = WaitUntil([&] {
        client.Poll();
        return client.IsConnected();
    });

    EXPECT_TRUE(connected);
    EXPECT_FALSE(client.IsConnecting());
}

TEST_F(TCPClientTest, SendBeforeFullyConnectedIsQueuedAndFlushedOnPoll) {
    auto listener = RawListener::Create();
    ASSERT_TRUE(listener.has_value());

    TCPClient client;
    ASSERT_TRUE(client.Connect(NetworkAddress("127.0.0.1", listener->port)));

    const auto frame = BuildFrame("queued while connecting");
    EXPECT_TRUE(client.Send(frame));

    auto accepted = listener->WaitForAccept();
    ASSERT_TRUE(accepted.has_value());

    WaitUntil([&] {
        client.Poll();
        return client.IsConnected();
    });

    std::array<Nexus::byte, 256> buffer{};
    std::optional<Nexus::usize> received;
    const bool got = WaitUntil([&] {
        std::optional<NetworkError> error;
        received = accepted->ReceiveInto(buffer, error);
        return received.has_value();
    });

    ASSERT_TRUE(got);
    ASSERT_EQ(*received, frame.size());
    EXPECT_TRUE(std::equal(frame.begin(), frame.end(), buffer.begin()));
}

TEST_F(TCPClientTest, PollDeliversFramesSentByPeer) {
    auto listener = RawListener::Create();
    ASSERT_TRUE(listener.has_value());

    TCPClient client;
    ASSERT_TRUE(client.Connect(NetworkAddress("127.0.0.1", listener->port)));

    auto accepted = listener->WaitForAccept();
    ASSERT_TRUE(accepted.has_value());
    ASSERT_TRUE(accepted->SetNonBlocking(true));

    const auto frame = BuildFrame("hello from peer");
    std::optional<NetworkError> sendError;
    ASSERT_TRUE(accepted->Send(frame, sendError).has_value());

    std::vector<std::vector<Nexus::byte>> received;
    const bool got = WaitUntil([&] {
        auto frames = client.Poll();
        received.insert(received.end(), std::make_move_iterator(frames.begin()), std::make_move_iterator(frames.end()));
        return !received.empty();
    });

    ASSERT_TRUE(got);
    ASSERT_EQ(received.size(), 1u);
    EXPECT_EQ(received.front(), frame);
}

TEST_F(TCPClientTest, PollDeliversMultipleQueuedFrames) {
    auto listener = RawListener::Create();
    ASSERT_TRUE(listener.has_value());

    TCPClient client;
    ASSERT_TRUE(client.Connect(NetworkAddress("127.0.0.1", listener->port)));

    auto accepted = listener->WaitForAccept();
    ASSERT_TRUE(accepted.has_value());
    ASSERT_TRUE(accepted->SetNonBlocking(true));

    const auto first = BuildFrame("first");
    const auto second = BuildFrame("second");

    std::vector<Nexus::byte> combined;
    combined.insert(combined.end(), first.begin(), first.end());
    combined.insert(combined.end(), second.begin(), second.end());

    std::optional<NetworkError> sendError;
    ASSERT_TRUE(accepted->Send(combined, sendError).has_value());

    std::vector<std::vector<Nexus::byte>> received;
    WaitUntil([&] {
        auto frames = client.Poll();
        received.insert(received.end(), std::make_move_iterator(frames.begin()), std::make_move_iterator(frames.end()));
        return received.size() >= 2;
    });

    ASSERT_EQ(received.size(), 2u);
    EXPECT_EQ(received[0], first);
    EXPECT_EQ(received[1], second);
}

TEST_F(TCPClientTest, DisconnectsWhenPeerCloses) {
    auto listener = RawListener::Create();
    ASSERT_TRUE(listener.has_value());

    TCPClient client;
    ASSERT_TRUE(client.Connect(NetworkAddress("127.0.0.1", listener->port)));

    auto accepted = listener->WaitForAccept();
    ASSERT_TRUE(accepted.has_value());

    WaitUntil([&] {
        client.Poll();
        return client.IsConnected();
    });
    ASSERT_TRUE(client.IsConnected());

    accepted->Close();

    const bool disconnected = WaitUntil([&] {
        client.Poll();
        return !client.IsConnected();
    });

    EXPECT_TRUE(disconnected);
}

TEST_F(TCPClientTest, SendOnDisconnectedClientFails) {
    TCPClient client;
    const auto frame = BuildFrame("nobody's listening");

    EXPECT_FALSE(client.Send(frame));
}

TEST_F(TCPClientTest, SendEmptyFrameFails) {
    auto listener = RawListener::Create();
    ASSERT_TRUE(listener.has_value());

    TCPClient client;
    ASSERT_TRUE(client.Connect(NetworkAddress("127.0.0.1", listener->port)));

    EXPECT_FALSE(client.Send(std::vector<Nexus::byte>{}));
}

TEST_F(TCPClientTest, DisconnectResetsState) {
    auto listener = RawListener::Create();
    ASSERT_TRUE(listener.has_value());

    TCPClient client;
    ASSERT_TRUE(client.Connect(NetworkAddress("127.0.0.1", listener->port)));

    auto accepted = listener->WaitForAccept();
    ASSERT_TRUE(accepted.has_value());
    WaitUntil([&] {
        client.Poll();
        return client.IsConnected();
    });
    ASSERT_TRUE(client.IsConnected());

    client.Disconnect();

    EXPECT_FALSE(client.IsConnected());
    EXPECT_FALSE(client.IsConnecting());
    EXPECT_TRUE(client.Poll().empty());
}

TEST_F(TCPClientTest, ConstructedFromAcceptedSocketIsImmediatelyConnected) {
    auto listener = RawListener::Create();
    ASSERT_TRUE(listener.has_value());

    Socket rawClient(SocketType::TCP);
    ASSERT_TRUE(rawClient.SetNonBlocking(true));
    std::optional<NetworkError> connectError;
    ASSERT_TRUE(rawClient.Connect(NetworkAddress("127.0.0.1", listener->port), connectError));

    auto accepted = listener->WaitForAccept();
    ASSERT_TRUE(accepted.has_value());

    TCPClient serverSideClient(std::move(*accepted));
    EXPECT_TRUE(serverSideClient.IsConnected());
    EXPECT_FALSE(serverSideClient.IsConnecting());
}

TEST_F(TCPClientTest, ReconnectAfterDisconnectStartsFresh) {
    auto listener = RawListener::Create();
    ASSERT_TRUE(listener.has_value());

    TCPClient client;
    ASSERT_TRUE(client.Connect(NetworkAddress("127.0.0.1", listener->port)));

    auto firstAccepted = listener->WaitForAccept();
    ASSERT_TRUE(firstAccepted.has_value());
    WaitUntil([&] {
        client.Poll();
        return client.IsConnected();
    });
    client.Disconnect();

    // Reconnect to the same listener.
    ASSERT_TRUE(client.Connect(NetworkAddress("127.0.0.1", listener->port)));
    auto secondAccepted = listener->WaitForAccept();
    ASSERT_TRUE(secondAccepted.has_value());

    const bool reconnected = WaitUntil([&] {
        client.Poll();
        return client.IsConnected();
    });
    EXPECT_TRUE(reconnected);
}
