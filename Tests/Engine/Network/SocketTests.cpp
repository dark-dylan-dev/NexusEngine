// SPDX-License-Identifier: MIT

#include <gtest/gtest.h>

import NE.Engine.Network.Common.NetworkAddress;
import NE.Engine.Network.Common.NetworkError;
import NE.Engine.Network.Platform.Socket;

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
        // avoids collisions on ports that are still active/waiting to be closed
        static std::atomic<Nexus::uint16> counter{51000};
        return counter.fetch_add(1);
    }

    class SocketTest : public ::testing::Test {};
} // namespace

// ---- Construction / validity -------------------------------------------

TEST_F(SocketTest, DefaultConstructedIsInvalid) {
    const Socket socket;
    EXPECT_FALSE(socket.IsValid());
}

TEST_F(SocketTest, ConstructedWithTypeIsValid) {
    const Socket tcp(SocketType::TCP);
    EXPECT_TRUE(tcp.IsValid());

    const Socket udp(SocketType::UDP);
    EXPECT_TRUE(udp.IsValid());
}

TEST_F(SocketTest, CloseInvalidatesSocket) {
    Socket socket(SocketType::TCP);
    ASSERT_TRUE(socket.IsValid());

    socket.Close();
    EXPECT_FALSE(socket.IsValid());
}

TEST_F(SocketTest, CloseOnAlreadyInvalidSocketIsSafe) {
    Socket socket;
    EXPECT_NO_THROW(socket.Close());
}

TEST_F(SocketTest, MoveConstructionTransfersValidity) {
    Socket original(SocketType::TCP);
    ASSERT_TRUE(original.IsValid());

    Socket moved(std::move(original));
    EXPECT_TRUE(moved.IsValid());
    EXPECT_FALSE(original.IsValid());
}

TEST_F(SocketTest, MoveAssignmentTransfersValidityAndClosesTarget) {
    Socket target(SocketType::TCP);
    Socket source(SocketType::UDP);
    ASSERT_TRUE(target.IsValid());
    ASSERT_TRUE(source.IsValid());

    target = std::move(source);
    EXPECT_TRUE(target.IsValid());
    EXPECT_FALSE(source.IsValid());
}

// ---- Bind / Listen on an invalid socket ---------------------------------

TEST_F(SocketTest, BindOnInvalidSocketFails) {
    Socket socket;
    EXPECT_FALSE(socket.Bind(0));
}

TEST_F(SocketTest, ListenOnInvalidSocketFails) {
    Socket socket;
    EXPECT_FALSE(socket.Listen());
}

TEST_F(SocketTest, SetNonBlockingOnInvalidSocketFails) {
    Socket socket;
    EXPECT_FALSE(socket.SetNonBlocking(true));
}

// ---- Operation/type guards ------------------------------------------------

TEST_F(SocketTest, ConnectOnUdpSocketFailsWithInvalidOperation) {
    Socket udp(SocketType::UDP);
    std::optional<NetworkError> error;

    EXPECT_FALSE(udp.Connect(NetworkAddress("127.0.0.1", 12345), error));
    ASSERT_TRUE(error.has_value());
    EXPECT_EQ(error->code, NetworkErrorCode::InvalidOperation);
}

TEST_F(SocketTest, SendToOnTcpSocketFailsWithInvalidOperation) {
    Socket tcp(SocketType::TCP);
    std::optional<NetworkError> error;
    const std::array<Nexus::byte, 4> data{};

    const auto result = tcp.SendTo(NetworkAddress("127.0.0.1", 12345), data, error);
    EXPECT_FALSE(result.has_value());
    ASSERT_TRUE(error.has_value());
    EXPECT_EQ(error->code, NetworkErrorCode::InvalidOperation);
}

TEST_F(SocketTest, SendOnUdpSocketFailsWithInvalidOperation) {
    Socket udp(SocketType::UDP);
    std::optional<NetworkError> error;
    const std::array<Nexus::byte, 4> data{};

    const auto result = udp.Send(data, error);
    EXPECT_FALSE(result.has_value());
    ASSERT_TRUE(error.has_value());
    EXPECT_EQ(error->code, NetworkErrorCode::InvalidOperation);
}

TEST_F(SocketTest, ListenOnUdpSocketFails) {
    Socket udp(SocketType::UDP);
    EXPECT_FALSE(udp.Listen());
}

TEST_F(SocketTest, AcceptOnUdpSocketReturnsNullopt) {
    Socket udp(SocketType::UDP);
    EXPECT_FALSE(udp.Accept().has_value());
}

// ---- TCP: bind / listen / connect / accept -------------------------------

TEST_F(SocketTest, BindThenListenSucceeds) {
    Socket listener(SocketType::TCP);
    ASSERT_TRUE(listener.IsValid());
    ASSERT_TRUE(listener.SetNonBlocking(true));

    const Nexus::uint16 port = NextTestPort();
    ASSERT_TRUE(listener.Bind(port));
    EXPECT_TRUE(listener.Listen());
}

TEST_F(SocketTest, AcceptOnListenerWithNoPendingConnectionReturnsNullopt) {
    Socket listener(SocketType::TCP);
    ASSERT_TRUE(listener.SetNonBlocking(true));
    const Nexus::uint16 port = NextTestPort();
    ASSERT_TRUE(listener.Bind(port));
    ASSERT_TRUE(listener.Listen());

    EXPECT_FALSE(listener.Accept().has_value());
}

TEST_F(SocketTest, ClientCanConnectAndServerCanAccept) {
    const Nexus::uint16 port = NextTestPort();

    Socket listener(SocketType::TCP);
    ASSERT_TRUE(listener.SetNonBlocking(true));
    ASSERT_TRUE(listener.Bind(port));
    ASSERT_TRUE(listener.Listen());

    Socket client(SocketType::TCP);
    ASSERT_TRUE(client.SetNonBlocking(true));

    std::optional<NetworkError> connectError;
    EXPECT_TRUE(client.Connect(NetworkAddress("127.0.0.1", port), connectError));

    std::optional<Socket> accepted;
    const bool got = WaitUntil([&] {
        if (!accepted) {
            accepted = listener.Accept();
        }
        return accepted.has_value();
    });

    ASSERT_TRUE(got);
    EXPECT_TRUE(accepted->IsValid());
}

TEST_F(SocketTest, ConnectToNonBlockingSocketReportsConnectionInProgressOrSucceeds) {
    const Nexus::uint16 port = NextTestPort();

    Socket listener(SocketType::TCP);
    ASSERT_TRUE(listener.SetNonBlocking(true));
    ASSERT_TRUE(listener.Bind(port));
    ASSERT_TRUE(listener.Listen());

    Socket client(SocketType::TCP);
    ASSERT_TRUE(client.SetNonBlocking(true));

    std::optional<NetworkError> error;
    const bool started = client.Connect(NetworkAddress("127.0.0.1", port), error);

    EXPECT_TRUE(started);
    if (error.has_value()) {
        EXPECT_EQ(error->code, NetworkErrorCode::ConnectionInProgress);
    }
}

TEST_F(SocketTest, CompleteConnectEventuallySucceeds) {
    const Nexus::uint16 port = NextTestPort();

    Socket listener(SocketType::TCP);
    ASSERT_TRUE(listener.SetNonBlocking(true));
    ASSERT_TRUE(listener.Bind(port));
    ASSERT_TRUE(listener.Listen());

    Socket client(SocketType::TCP);
    ASSERT_TRUE(client.SetNonBlocking(true));
    std::optional<NetworkError> connectError;
    ASSERT_TRUE(client.Connect(NetworkAddress("127.0.0.1", port), connectError));

    const bool completed = WaitUntil([&] {
        static_cast<void>(listener.Accept());
        std::optional<NetworkError> completeError;
        return client.CompleteConnect(completeError);
    });

    EXPECT_TRUE(completed);
}

TEST_F(SocketTest, ConnectToClosedPortFails) {
    Socket client(SocketType::TCP);
    ASSERT_TRUE(client.SetNonBlocking(true));

    // Nothing is listening on this port.
    const Nexus::uint16 port = NextTestPort();
    std::optional<NetworkError> connectError;
    const bool started = client.Connect(NetworkAddress("127.0.0.1", port), connectError);

    if (!started) {
        ASSERT_TRUE(connectError.has_value());
        return;
    }

    bool sawSpuriousSuccess = false;
    WaitUntil([&] {
        std::optional<NetworkError> completeError;
        if (client.CompleteConnect(completeError)) {
            sawSpuriousSuccess = true;
            return true;
        }
        return completeError && completeError->code != NetworkErrorCode::ConnectionInProgress;
    });

    EXPECT_FALSE(sawSpuriousSuccess);
}

// ---- TCP: send / receive --------------------------------------------------

namespace {
    struct ConnectedPair {
        Socket client;
        Socket server;
    };

    std::optional<ConnectedPair> MakeConnectedPair(Nexus::uint16 port) {
        Socket listener(SocketType::TCP);
        if (!listener.SetNonBlocking(true) || !listener.Bind(port) || !listener.Listen()) {
            return std::nullopt;
        }

        Socket client(SocketType::TCP);
        if (!client.SetNonBlocking(true)) {
            return std::nullopt;
        }

        std::optional<NetworkError> connectError;
        if (!client.Connect(NetworkAddress("127.0.0.1", port), connectError)) {
            return std::nullopt;
        }

        std::optional<Socket> server;
        const bool connected = WaitUntil([&] {
            if (!server) {
                server = listener.Accept();
            }
            std::optional<NetworkError> completeError;
            const bool clientReady = client.CompleteConnect(completeError);
            return server.has_value() && clientReady;
        });

        if (!connected || !server) {
            return std::nullopt;
        }

        return ConnectedPair{std::move(client), std::move(*server)};
    }
} // namespace

TEST_F(SocketTest, SendThenReceiveDeliversData) {
    auto pair = MakeConnectedPair(NextTestPort());
    ASSERT_TRUE(pair.has_value());

    const std::array<Nexus::byte, 5> payload{Nexus::byte{'h'}, Nexus::byte{'e'}, Nexus::byte{'l'}, Nexus::byte{'l'},
                                             Nexus::byte{'o'}};
    std::optional<NetworkError> sendError;
    const auto sent = pair->client.Send(payload, sendError);
    ASSERT_TRUE(sent.has_value());
    EXPECT_EQ(*sent, payload.size());

    std::array<Nexus::byte, 64> buffer{};
    std::optional<Nexus::usize> received;
    const bool got = WaitUntil([&] {
        std::optional<NetworkError> receiveError;
        received = pair->server.ReceiveInto(buffer, receiveError);
        return received.has_value();
    });

    ASSERT_TRUE(got);
    ASSERT_EQ(*received, payload.size());
    EXPECT_TRUE(std::equal(payload.begin(), payload.end(), buffer.begin()));
}

TEST_F(SocketTest, ReceiveOnSocketWithNoDataReturnsWouldBlock) {
    auto pair = MakeConnectedPair(NextTestPort());
    ASSERT_TRUE(pair.has_value());

    std::array<Nexus::byte, 64> buffer{};
    std::optional<NetworkError> error;
    const auto received = pair->server.ReceiveInto(buffer, error);

    EXPECT_FALSE(received.has_value());
    ASSERT_TRUE(error.has_value());
    EXPECT_EQ(error->code, NetworkErrorCode::WouldBlock);
}

TEST_F(SocketTest, ReceiveAfterPeerClosesReportsConnectionClosed) {
    auto pair = MakeConnectedPair(NextTestPort());
    ASSERT_TRUE(pair.has_value());

    pair->client.Close();

    std::array<Nexus::byte, 64> buffer{};
    std::optional<Nexus::usize> received;
    std::optional<NetworkError> error;
    const bool sawClose = WaitUntil([&] {
        received = pair->server.ReceiveInto(buffer, error);
        return !received.has_value() && error.has_value() && error->code == NetworkErrorCode::ConnectionClosed;
    });

    EXPECT_TRUE(sawClose);
}

TEST_F(SocketTest, SendEmptySpanSucceedsWithZeroBytes) {
    auto pair = MakeConnectedPair(NextTestPort());
    ASSERT_TRUE(pair.has_value());

    std::optional<NetworkError> error;
    const auto sent = pair->client.Send(std::span<const Nexus::byte>{}, error);

    ASSERT_TRUE(sent.has_value());
    EXPECT_EQ(*sent, 0u);
    EXPECT_FALSE(error.has_value());
}

// ---- UDP: bind / send / receive -------------------------------------------

TEST_F(SocketTest, UdpBindSucceeds) {
    Socket udp(SocketType::UDP);
    ASSERT_TRUE(udp.SetNonBlocking(true));
    EXPECT_TRUE(udp.Bind(NextTestPort()));
}

TEST_F(SocketTest, UdpSendToThenReceiveFromDeliversDatagram) {
    const Nexus::uint16 serverPort = NextTestPort();

    Socket server(SocketType::UDP);
    ASSERT_TRUE(server.SetNonBlocking(true));
    ASSERT_TRUE(server.Bind(serverPort));

    Socket client(SocketType::UDP);
    ASSERT_TRUE(client.SetNonBlocking(true));
    ASSERT_TRUE(client.Bind(0));

    const std::array<Nexus::byte, 3> payload{Nexus::byte{1}, Nexus::byte{2}, Nexus::byte{3}};
    std::optional<NetworkError> sendError;
    const auto sent = client.SendTo(NetworkAddress("127.0.0.1", serverPort), payload, sendError);
    ASSERT_TRUE(sent.has_value());
    EXPECT_EQ(*sent, payload.size());

    std::array<Nexus::byte, 64> buffer{};
    NetworkAddress sender;
    std::optional<Nexus::usize> received;
    const bool got = WaitUntil([&] {
        std::optional<NetworkError> receiveError;
        received = server.ReceiveFrom(buffer, sender, receiveError);
        return received.has_value();
    });

    ASSERT_TRUE(got);
    ASSERT_EQ(*received, payload.size());
    EXPECT_TRUE(std::equal(payload.begin(), payload.end(), buffer.begin()));
    EXPECT_EQ(sender.Host(), "127.0.0.1");
}

TEST_F(SocketTest, UdpReceiveWithNoDataReturnsWouldBlock) {
    Socket udp(SocketType::UDP);
    ASSERT_TRUE(udp.SetNonBlocking(true));
    ASSERT_TRUE(udp.Bind(NextTestPort()));

    std::array<Nexus::byte, 64> buffer{};
    NetworkAddress sender;
    std::optional<NetworkError> error;
    const auto received = udp.ReceiveFrom(buffer, sender, error);

    EXPECT_FALSE(received.has_value());
    ASSERT_TRUE(error.has_value());
    EXPECT_EQ(error->code, NetworkErrorCode::WouldBlock);
}

TEST_F(SocketTest, UdpSenderAddressMatchesClientBoundPort) {
    const Nexus::uint16 serverPort = NextTestPort();
    const Nexus::uint16 clientPort = NextTestPort();

    Socket server(SocketType::UDP);
    ASSERT_TRUE(server.SetNonBlocking(true));
    ASSERT_TRUE(server.Bind(serverPort));

    Socket client(SocketType::UDP);
    ASSERT_TRUE(client.SetNonBlocking(true));
    ASSERT_TRUE(client.Bind(clientPort));

    const std::array<Nexus::byte, 1> payload{Nexus::byte{7}};
    std::optional<NetworkError> sendError;
    ASSERT_TRUE(client.SendTo(NetworkAddress("127.0.0.1", serverPort), payload, sendError).has_value());

    std::array<Nexus::byte, 64> buffer{};
    NetworkAddress sender;
    std::optional<Nexus::usize> received;
    const bool got = WaitUntil([&] {
        std::optional<NetworkError> receiveError;
        received = server.ReceiveFrom(buffer, sender, receiveError);
        return received.has_value();
    });

    ASSERT_TRUE(got);
    EXPECT_EQ(sender.Port(), clientPort);
}
