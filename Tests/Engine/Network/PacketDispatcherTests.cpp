// SPDX-License-Identifier: MIT

#include <gtest/gtest.h>

import NE.Engine.Network.Packet;
import NE.Engine.Network.PacketWriter;
import NE.Engine.Network.Protocol.PacketDispatcher;

import NE.Engine.Core.Types;

import std;

using namespace Nexus::Network;

namespace {
    class PacketDispatcherTest : public ::testing::Test {
    protected:
        PacketDispatcher m_dispatcher;
    };

    std::vector<Nexus::byte> BuildChatFrame(Nexus::uint32 sequence, std::string_view text) {
        PacketWriter writer(PacketType::ChatMessage, sequence);
        writer.WriteString(text);
        return writer.Build();
    }
} // namespace

TEST_F(PacketDispatcherTest, HasHandlerFalseWhenNoneRegistered) {
    EXPECT_FALSE(m_dispatcher.HasHandler(PacketType::ChatMessage));
}

TEST_F(PacketDispatcherTest, RegisterMakesHasHandlerTrue) {
    m_dispatcher.Register(PacketType::ChatMessage, [](Packet&) {});
    EXPECT_TRUE(m_dispatcher.HasHandler(PacketType::ChatMessage));
}

TEST_F(PacketDispatcherTest, RegisterWithEmptyHandlerUnregisters) {
    m_dispatcher.Register(PacketType::ChatMessage, [](Packet&) {});
    ASSERT_TRUE(m_dispatcher.HasHandler(PacketType::ChatMessage));

    m_dispatcher.Register(PacketType::ChatMessage, PacketDispatcher::Handler{});
    EXPECT_FALSE(m_dispatcher.HasHandler(PacketType::ChatMessage));
}

TEST_F(PacketDispatcherTest, UnregisterRemovesHandler) {
    m_dispatcher.Register(PacketType::ChatMessage, [](Packet&) {});
    m_dispatcher.Unregister(PacketType::ChatMessage);

    EXPECT_FALSE(m_dispatcher.HasHandler(PacketType::ChatMessage));
}

TEST_F(PacketDispatcherTest, ClearRemovesAllHandlers) {
    m_dispatcher.Register(PacketType::ChatMessage, [](Packet&) {});
    m_dispatcher.Register(PacketType::PlayerInput, [](Packet&) {});

    m_dispatcher.Clear();

    EXPECT_FALSE(m_dispatcher.HasHandler(PacketType::ChatMessage));
    EXPECT_FALSE(m_dispatcher.HasHandler(PacketType::PlayerInput));
}

TEST_F(PacketDispatcherTest, DispatchInvokesRegisteredHandler) {
    bool invoked = false;
    m_dispatcher.Register(PacketType::ChatMessage, [&invoked](Packet&) { invoked = true; });

    const auto frame = BuildChatFrame(1, "hi");
    m_dispatcher.Dispatch(frame);

    EXPECT_TRUE(invoked);
}

TEST_F(PacketDispatcherTest, DispatchPassesReadablePayloadToHandler) {
    std::string received;
    m_dispatcher.Register(PacketType::ChatMessage,
                          [&received](Packet& packet) { static_cast<void>(packet.ReadString(received)); });

    const auto frame = BuildChatFrame(1, "hello world");
    m_dispatcher.Dispatch(frame);

    EXPECT_EQ(received, "hello world");
}

TEST_F(PacketDispatcherTest, DispatchDoesNotInvokeHandlerForDifferentType) {
    bool chatInvoked = false;
    bool inputInvoked = false;
    m_dispatcher.Register(PacketType::ChatMessage, [&chatInvoked](Packet&) { chatInvoked = true; });
    m_dispatcher.Register(PacketType::PlayerInput, [&inputInvoked](Packet&) { inputInvoked = true; });

    const auto frame = BuildChatFrame(1, "hi");
    m_dispatcher.Dispatch(frame);

    EXPECT_TRUE(chatInvoked);
    EXPECT_FALSE(inputInvoked);
}

TEST_F(PacketDispatcherTest, DispatchIgnoresFrameWithNoRegisteredHandler) {
    const auto frame = BuildChatFrame(1, "hi");
    EXPECT_NO_THROW(m_dispatcher.Dispatch(frame));
}

TEST_F(PacketDispatcherTest, DispatchIgnoresFrameShorterThanHeader) {
    bool invoked = false;
    m_dispatcher.Register(PacketType::ChatMessage, [&invoked](Packet&) { invoked = true; });

    const std::vector<Nexus::byte> tooShort(HEADER_SIZE - 1, Nexus::byte{0});
    m_dispatcher.Dispatch(tooShort);

    EXPECT_FALSE(invoked);
}

TEST_F(PacketDispatcherTest, DispatchIgnoresFrameWithMismatchedPayloadSize) {
    bool invoked = false;
    m_dispatcher.Register(PacketType::ChatMessage, [&invoked](Packet&) { invoked = true; });

    auto frame = BuildChatFrame(1, "hello world");
    Nexus::usize offset = 0;
    const auto corruptedSize = Wire::HostToBigEndian(Nexus::uint32{9999});
    std::memcpy(frame.data() + offset, &corruptedSize, sizeof(corruptedSize));

    m_dispatcher.Dispatch(frame);

    EXPECT_FALSE(invoked);
}

TEST_F(PacketDispatcherTest, DispatchIgnoresFrameWithUnknownType) {
    bool invoked = false;
    m_dispatcher.Register(PacketType::ChatMessage, [&invoked](Packet&) { invoked = true; });

    auto frame = BuildChatFrame(1, "hi");

    Nexus::usize offset = sizeof(Nexus::uint32);
    const auto bogusType = Wire::HostToBigEndian(Nexus::uint16{9999});
    std::memcpy(frame.data() + offset, &bogusType, sizeof(bogusType));

    EXPECT_NO_THROW(m_dispatcher.Dispatch(frame));
    EXPECT_FALSE(invoked);
}

TEST_F(PacketDispatcherTest, DispatchAcceptsSpanOverload) {
    bool invoked = false;
    m_dispatcher.Register(PacketType::ChatMessage, [&invoked](Packet&) { invoked = true; });

    const auto frame = BuildChatFrame(1, "hi");
    m_dispatcher.Dispatch(std::span<const Nexus::byte>(frame));

    EXPECT_TRUE(invoked);
}

TEST_F(PacketDispatcherTest, DispatchWithEmptyPayloadInvokesHandler) {
    bool invoked = false;
    m_dispatcher.Register(PacketType::ClientHello, [&invoked](Packet&) { invoked = true; });

    PacketWriter writer(PacketType::ClientHello, 1);
    const auto frame = writer.Build();
    m_dispatcher.Dispatch(frame);

    EXPECT_TRUE(invoked);
}

TEST_F(PacketDispatcherTest, ReRegisteringReplacesPreviousHandler) {
    int callCount = 0;
    m_dispatcher.Register(PacketType::ChatMessage, [&callCount](Packet&) { callCount += 1; });
    m_dispatcher.Register(PacketType::ChatMessage, [&callCount](Packet&) { callCount += 100; });

    const auto frame = BuildChatFrame(1, "hi");
    m_dispatcher.Dispatch(frame);

    EXPECT_EQ(callCount, 100);
}
