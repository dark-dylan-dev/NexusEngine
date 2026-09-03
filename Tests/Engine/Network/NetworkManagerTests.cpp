// SPDX-License-Identifier: MIT

#include <gtest/gtest.h>

import NE.Engine.Network.Common.NetworkAddress;
import NE.Engine.Network.Common.NetworkError;
import NE.Engine.Network.Manager;
import NE.Engine.Network.Packet;
import NE.Engine.Network.PacketWriter;
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
        static std::atomic<Nexus::uint16> counter{55000};
        return counter.fetch_add(1);
    }

    // A raw-Socket "server" used as the peer for NetworkManager's TCP side,
    // so NetworkManager is tested in isolation rather than against TCPServer.
    struct RawServer {
        Socket listener;
        Nexus::uint16 port = 0;
        std::optional<Socket> accepted;

        static std::optional<RawServer> Create() {
            RawServer result;
            result.port = NextTestPort();
            result.listener = Socket(SocketType::TCP);
            if (!result.listener.IsValid() || !result.listener.SetNonBlocking(true) ||
                !result.listener.Bind(result.port) || !result.listener.Listen()) {
                return std::nullopt;
            }
            return result;
        }

        bool WaitForAccept() {
            return WaitUntil([&] {
                bool nonBlocking = false;
                if (!accepted) {
                    accepted = listener.Accept();
                    if (accepted) {
                        nonBlocking = accepted->SetNonBlocking(true);
                    }
                }
                return accepted.has_value() && nonBlocking;
            });
        }
    };

    class NetworkManagerTest : public ::testing::Test {};
} // namespace

TEST_F(NetworkManagerTest, DefaultConstructedIsNotConnected) {
    const NetworkManager manager;
    EXPECT_FALSE(manager.IsConnected());
    EXPECT_FALSE(manager.IsConnecting());
}

TEST_F(NetworkManagerTest, StartClientBeginsConnecting) {
    auto server = RawServer::Create();
    ASSERT_TRUE(server.has_value());

    NetworkManager manager;
    EXPECT_TRUE(manager.StartClient(NetworkAddress("127.0.0.1", server->port)));
}

TEST_F(NetworkManagerTest, UpdateCompletesConnectionAfterAccept) {
    auto server = RawServer::Create();
    ASSERT_TRUE(server.has_value());

    NetworkManager manager;
    ASSERT_TRUE(manager.StartClient(NetworkAddress("127.0.0.1", server->port)));
    ASSERT_TRUE(server->WaitForAccept());

    const bool connected = WaitUntil([&] {
        manager.Update();
        return manager.IsConnected();
    });

    EXPECT_TRUE(connected);
    EXPECT_FALSE(manager.IsConnecting());
}

TEST_F(NetworkManagerTest, RegisterHandlerReceivesDispatchedFrameFromPeer) {
    auto server = RawServer::Create();
    ASSERT_TRUE(server.has_value());

    NetworkManager manager;

    std::string receivedText;
    manager.RegisterHandler(PacketType::ChatMessage,
                            [&receivedText](Packet& packet) { static_cast<void>(packet.ReadString(receivedText)); });

    ASSERT_TRUE(manager.StartClient(NetworkAddress("127.0.0.1", server->port)));
    ASSERT_TRUE(server->WaitForAccept());

    WaitUntil([&] {
        manager.Update();
        return manager.IsConnected();
    });
    ASSERT_TRUE(manager.IsConnected());

    PacketWriter writer(PacketType::ChatMessage, 1);
    writer.WriteString("greetings from the server");
    const auto frame = writer.Build();

    std::optional<NetworkError> sendError;
    ASSERT_TRUE(server->accepted->Send(frame, sendError).has_value());

    const bool got = WaitUntil([&] {
        manager.Update();
        return !receivedText.empty();
    });

    ASSERT_TRUE(got);
    EXPECT_EQ(receivedText, "greetings from the server");
}

TEST_F(NetworkManagerTest, SendReliableDeliversFrameToPeer) {
    auto server = RawServer::Create();
    ASSERT_TRUE(server.has_value());

    NetworkManager manager;
    ASSERT_TRUE(manager.StartClient(NetworkAddress("127.0.0.1", server->port)));
    ASSERT_TRUE(server->WaitForAccept());

    WaitUntil([&] {
        manager.Update();
        return manager.IsConnected();
    });
    ASSERT_TRUE(manager.IsConnected());

    PacketWriter writer = manager.MakeWriter(PacketType::ChatMessage);
    writer.WriteString("via SendReliable");
    const auto frame = writer.Build();

    EXPECT_TRUE(manager.SendReliable(frame));

    std::array<Nexus::byte, 256> buffer{};
    std::optional<Nexus::usize> received;
    const bool got = WaitUntil([&] {
        std::optional<NetworkError> error;
        received = server->accepted->ReceiveInto(buffer, error);
        return received.has_value();
    });

    ASSERT_TRUE(got);
    ASSERT_EQ(*received, frame.size());
    EXPECT_TRUE(std::equal(frame.begin(), frame.end(), buffer.begin()));
}

TEST_F(NetworkManagerTest, MakeWriterAssignsIncrementingSequenceNumbers) {
    NetworkManager manager;

    PacketWriter first = manager.MakeWriter(PacketType::PlayerInput);
    const auto firstFrame = first.Build();

    PacketWriter second = manager.MakeWriter(PacketType::PlayerInput);
    const auto secondFrame = second.Build();

    Nexus::usize firstOffset = sizeof(Nexus::uint32) + sizeof(Nexus::uint16);
    Nexus::usize secondOffset = firstOffset;
    Nexus::uint32 firstSequence = 0;
    Nexus::uint32 secondSequence = 0;

    ASSERT_TRUE(Wire::Read(firstFrame, firstOffset, firstSequence));
    ASSERT_TRUE(Wire::Read(secondFrame, secondOffset, secondSequence));

    EXPECT_EQ(secondSequence, firstSequence + 1);
}

TEST_F(NetworkManagerTest, SendReliableBeforeConnectingFails) {
    NetworkManager manager;

    PacketWriter writer = manager.MakeWriter(PacketType::ChatMessage);
    writer.WriteString("nobody there yet");

    EXPECT_FALSE(manager.SendReliable(writer.Build()));
}

TEST_F(NetworkManagerTest, UnregisteredPacketTypeIsSilentlyIgnored) {
    auto server = RawServer::Create();
    ASSERT_TRUE(server.has_value());

    NetworkManager manager;
    // No handlers registered at all.
    ASSERT_TRUE(manager.StartClient(NetworkAddress("127.0.0.1", server->port)));
    ASSERT_TRUE(server->WaitForAccept());

    WaitUntil([&] {
        manager.Update();
        return manager.IsConnected();
    });
    ASSERT_TRUE(manager.IsConnected());

    PacketWriter writer(PacketType::SpawnEntity, 1);
    writer.Write(Nexus::uint32{1});
    const auto frame = writer.Build();

    std::optional<NetworkError> sendError;
    ASSERT_TRUE(server->accepted->Send(frame, sendError).has_value());

    EXPECT_NO_THROW({
        for (int i = 0; i < 20; ++i) {
            manager.Update();
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    });
}
