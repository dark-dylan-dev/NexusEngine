// SPDX-License-Identifier: MIT

#include <gtest/gtest.h>

import NE.Engine.Network.Common.NetworkAddress;
import NE.Engine.Network.Common.NetworkError;
import NE.Engine.Network.Packet;
import NE.Engine.Network.PacketWriter;
import NE.Engine.Network.Platform.Socket;
import NE.Engine.Network.TCP.TCPServer;

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
        static std::atomic<Nexus::uint16> counter{53000};
        return counter.fetch_add(1);
    }

    std::optional<Socket> ConnectRawClient(Nexus::uint16 port) {
        Socket client(SocketType::TCP);
        if (!client.IsValid() || !client.SetNonBlocking(true)) {
            return std::nullopt;
        }
        std::optional<NetworkError> error;
        if (!client.Connect(NetworkAddress("127.0.0.1", port), error)) {
            return std::nullopt;
        }

        const bool ready = WaitUntil([&] {
            std::optional<NetworkError> completeError;
            return client.CompleteConnect(completeError);
        });
        if (!ready) {
            return std::nullopt;
        }
        return client;
    }

    std::vector<Nexus::byte> BuildFrame(std::string_view text) {
        PacketWriter writer(PacketType::ChatMessage, 1);
        writer.WriteString(text);
        return writer.Build();
    }

    class TCPServerTest : public ::testing::Test {};
} // namespace

TEST_F(TCPServerTest, ListenSucceeds) {
    TCPServer server;
    EXPECT_TRUE(server.Listen(NextTestPort()));
}

TEST_F(TCPServerTest, ClientCountStartsAtZero) {
    TCPServer server;
    ASSERT_TRUE(server.Listen(NextTestPort()));
    EXPECT_EQ(server.ClientCount(), 0u);
}

TEST_F(TCPServerTest, AcceptPendingWithNoConnectionsReturnsEmpty) {
    TCPServer server;
    ASSERT_TRUE(server.Listen(NextTestPort()));
    EXPECT_TRUE(server.AcceptPending().empty());
}

TEST_F(TCPServerTest, AcceptsIncomingConnection) {
    TCPServer server;
    const Nexus::uint16 port = NextTestPort();
    ASSERT_TRUE(server.Listen(port));

    auto client = ConnectRawClient(port);
    ASSERT_TRUE(client.has_value());

    std::vector<ClientId> accepted;
    const bool got = WaitUntil([&] {
        auto ids = server.AcceptPending();
        accepted.insert(accepted.end(), ids.begin(), ids.end());
        return !accepted.empty();
    });

    ASSERT_TRUE(got);
    EXPECT_EQ(accepted.size(), 1u);
    EXPECT_EQ(server.ClientCount(), 1u);
}

TEST_F(TCPServerTest, ClientIdsAreUniqueAndIncrementing) {
    TCPServer server;
    const Nexus::uint16 port = NextTestPort();
    ASSERT_TRUE(server.Listen(port));

    auto firstClient = ConnectRawClient(port);
    ASSERT_TRUE(firstClient.has_value());
    std::vector<ClientId> firstIds;
    WaitUntil([&] {
        auto ids = server.AcceptPending();
        firstIds.insert(firstIds.end(), ids.begin(), ids.end());
        return !firstIds.empty();
    });
    ASSERT_EQ(firstIds.size(), 1u);

    auto secondClient = ConnectRawClient(port);
    ASSERT_TRUE(secondClient.has_value());
    std::vector<ClientId> secondIds;
    WaitUntil([&] {
        auto ids = server.AcceptPending();
        secondIds.insert(secondIds.end(), ids.begin(), ids.end());
        return !secondIds.empty();
    });
    ASSERT_EQ(secondIds.size(), 1u);

    EXPECT_NE(firstIds.front(), secondIds.front());
    EXPECT_GT(secondIds.front(), firstIds.front());
}

TEST_F(TCPServerTest, PollAllReturnsFramesSentByClient) {
    TCPServer server;
    const Nexus::uint16 port = NextTestPort();
    ASSERT_TRUE(server.Listen(port));

    auto client = ConnectRawClient(port);
    ASSERT_TRUE(client.has_value());

    ClientId id = 0;
    WaitUntil([&] {
        auto ids = server.AcceptPending();
        if (!ids.empty()) {
            id = ids.front();
        }
        return id != 0;
    });
    ASSERT_NE(id, 0u);

    const auto frame = BuildFrame("hello server");
    std::optional<NetworkError> sendError;
    ASSERT_TRUE(client->Send(frame, sendError).has_value());

    std::vector<std::pair<ClientId, std::vector<Nexus::byte>>> received;
    const bool got = WaitUntil([&] {
        auto frames = server.PollAll();
        received.insert(received.end(), std::make_move_iterator(frames.begin()), std::make_move_iterator(frames.end()));
        return !received.empty();
    });

    ASSERT_TRUE(got);
    EXPECT_EQ(received.front().first, id);
    EXPECT_EQ(received.front().second, frame);
}

TEST_F(TCPServerTest, SendToDeliversFrameToCorrectClient) {
    TCPServer server;
    const Nexus::uint16 port = NextTestPort();
    ASSERT_TRUE(server.Listen(port));

    auto client = ConnectRawClient(port);
    ASSERT_TRUE(client.has_value());
    ASSERT_TRUE(client->SetNonBlocking(true));

    ClientId id = 0;
    WaitUntil([&] {
        auto ids = server.AcceptPending();
        if (!ids.empty()) {
            id = ids.front();
        }
        return id != 0;
    });
    ASSERT_NE(id, 0u);

    const auto frame = BuildFrame("hello client");
    EXPECT_TRUE(server.SendTo(id, frame));

    std::array<Nexus::byte, 256> buffer{};
    std::optional<Nexus::usize> received;
    const bool got = WaitUntil([&] {
        std::optional<NetworkError> error;
        received = client->ReceiveInto(buffer, error);
        return received.has_value();
    });

    ASSERT_TRUE(got);
    ASSERT_EQ(*received, frame.size());
    EXPECT_TRUE(std::equal(frame.begin(), frame.end(), buffer.begin()));
}

TEST_F(TCPServerTest, SendToUnknownClientIdFails) {
    TCPServer server;
    ASSERT_TRUE(server.Listen(NextTestPort()));

    const auto frame = BuildFrame("nobody home");
    EXPECT_FALSE(server.SendTo(999, frame));
}

TEST_F(TCPServerTest, BroadcastReachesAllConnectedClients) {
    TCPServer server;
    const Nexus::uint16 port = NextTestPort();
    ASSERT_TRUE(server.Listen(port));

    auto clientA = ConnectRawClient(port);
    auto clientB = ConnectRawClient(port);
    ASSERT_TRUE(clientA.has_value());
    ASSERT_TRUE(clientB.has_value());
    ASSERT_TRUE(clientA->SetNonBlocking(true));
    ASSERT_TRUE(clientB->SetNonBlocking(true));

    WaitUntil([&] {
        server.AcceptPending();
        return server.ClientCount() == 2;
    });
    ASSERT_EQ(server.ClientCount(), 2u);

    const auto frame = BuildFrame("hello everyone");
    server.Broadcast(frame);

    std::array<Nexus::byte, 256> bufferA{};
    std::array<Nexus::byte, 256> bufferB{};
    std::optional<Nexus::usize> receivedA;
    std::optional<Nexus::usize> receivedB;

    const bool gotBoth = WaitUntil([&] {
        std::optional<NetworkError> errorA;
        std::optional<NetworkError> errorB;
        if (!receivedA) {
            receivedA = clientA->ReceiveInto(bufferA, errorA);
        }
        if (!receivedB) {
            receivedB = clientB->ReceiveInto(bufferB, errorB);
        }
        return receivedA.has_value() && receivedB.has_value();
    });

    ASSERT_TRUE(gotBoth);
    EXPECT_TRUE(std::equal(frame.begin(), frame.end(), bufferA.begin()));
    EXPECT_TRUE(std::equal(frame.begin(), frame.end(), bufferB.begin()));
}

TEST_F(TCPServerTest, DisconnectedClientIsRemovedOnPollAll) {
    TCPServer server;
    const Nexus::uint16 port = NextTestPort();
    ASSERT_TRUE(server.Listen(port));

    auto client = ConnectRawClient(port);
    ASSERT_TRUE(client.has_value());

    WaitUntil([&] {
        server.AcceptPending();
        return server.ClientCount() == 1;
    });
    ASSERT_EQ(server.ClientCount(), 1u);

    client->Close();

    const bool removed = WaitUntil([&] {
        server.PollAll();
        return server.ClientCount() == 0;
    });

    EXPECT_TRUE(removed);
}

TEST_F(TCPServerTest, StopClearsClientsAndListenSocket) {
    TCPServer server;
    const Nexus::uint16 port = NextTestPort();
    ASSERT_TRUE(server.Listen(port));

    auto client = ConnectRawClient(port);
    ASSERT_TRUE(client.has_value());
    WaitUntil([&] {
        server.AcceptPending();
        return server.ClientCount() == 1;
    });
    ASSERT_EQ(server.ClientCount(), 1u);

    server.Stop();

    EXPECT_EQ(server.ClientCount(), 0u);
    EXPECT_TRUE(server.AcceptPending().empty());
}

TEST_F(TCPServerTest, ListenAgainResetsExistingClients) {
    TCPServer server;
    const Nexus::uint16 firstPort = NextTestPort();
    ASSERT_TRUE(server.Listen(firstPort));

    auto client = ConnectRawClient(firstPort);
    ASSERT_TRUE(client.has_value());
    WaitUntil([&] {
        server.AcceptPending();
        return server.ClientCount() == 1;
    });
    ASSERT_EQ(server.ClientCount(), 1u);

    const Nexus::uint16 secondPort = NextTestPort();
    ASSERT_TRUE(server.Listen(secondPort));

    EXPECT_EQ(server.ClientCount(), 0u);
}
