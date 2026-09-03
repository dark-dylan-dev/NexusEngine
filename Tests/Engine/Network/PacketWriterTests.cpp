// SPDX-License-Identifier: MIT

#include <gtest/gtest.h>

import NE.Engine.Network.Packet;
import NE.Engine.Network.PacketWriter;

import NE.Engine.Core.Types;

import std;

using namespace Nexus::Network;

TEST(PacketWriterTest, BuildProducesFrameWithCorrectHeader) {
    PacketWriter writer(PacketType::ChatMessage, 7);
    writer.Write(Nexus::uint32{123});

    const std::vector<Nexus::byte> frame = writer.Build();
    ASSERT_GE(frame.size(), HEADER_SIZE);

    Nexus::usize offset = 0;
    Nexus::uint32 payloadSize = 0;
    Nexus::uint16 type = 0;
    Nexus::uint32 sequence = 0;

    ASSERT_TRUE(Wire::Read(frame, offset, payloadSize));
    ASSERT_TRUE(Wire::Read(frame, offset, type));
    ASSERT_TRUE(Wire::Read(frame, offset, sequence));

    EXPECT_EQ(payloadSize, sizeof(Nexus::uint32));
    EXPECT_EQ(type, static_cast<Nexus::uint16>(PacketType::ChatMessage));
    EXPECT_EQ(sequence, 7u);
}

TEST(PacketWriterTest, BuildWithNoPayloadHasZeroPayloadSize) {
    PacketWriter writer(PacketType::ClientHello, 1);
    const std::vector<Nexus::byte> frame = writer.Build();

    ASSERT_EQ(frame.size(), HEADER_SIZE);

    Nexus::usize offset = 0;
    Nexus::uint32 payloadSize = 0;
    ASSERT_TRUE(Wire::Read(frame, offset, payloadSize));
    EXPECT_EQ(payloadSize, 0u);
}

TEST(PacketWriterTest, FrameSizeIsHeaderPlusPayload) {
    PacketWriter writer(PacketType::PlayerState, 1);
    writer.Write(Nexus::uint32{1});
    writer.Write(Nexus::uint32{2});
    writer.Write(Nexus::uint32{3});

    const std::vector<Nexus::byte> frame = writer.Build();
    EXPECT_EQ(frame.size(), HEADER_SIZE + 3 * sizeof(Nexus::uint32));
}

TEST(PacketWriterTest, PayloadBytesFollowHeaderInFrame) {
    PacketWriter writer(PacketType::PlayerInput, 1);
    writer.Write(Nexus::uint8{0xAB});

    const std::vector<Nexus::byte> frame = writer.Build();
    ASSERT_EQ(frame.size(), HEADER_SIZE + 1);
    EXPECT_EQ(static_cast<Nexus::uint8>(frame[HEADER_SIZE]), 0xAB);
}

TEST(PacketWriterTest, WriteStringEncodesLengthPrefixAndBytes) {
    PacketWriter writer(PacketType::ChatMessage, 1);
    ASSERT_TRUE(writer.WriteString("hi"));

    const std::vector<Nexus::byte> frame = writer.Build();
    ASSERT_EQ(frame.size(), HEADER_SIZE + sizeof(Nexus::uint32) + 2);

    Nexus::usize offset = HEADER_SIZE;
    Nexus::uint32 length = 0;
    ASSERT_TRUE(Wire::Read(frame, offset, length));
    EXPECT_EQ(length, 2u);

    EXPECT_EQ(static_cast<char>(frame[offset]), 'h');
    EXPECT_EQ(static_cast<char>(frame[offset + 1]), 'i');
}

TEST(PacketWriterTest, WriteStringHandlesEmptyString) {
    PacketWriter writer(PacketType::ChatMessage, 1);
    ASSERT_TRUE(writer.WriteString(""));

    const std::vector<Nexus::byte> frame = writer.Build();
    EXPECT_EQ(frame.size(), HEADER_SIZE + sizeof(Nexus::uint32));
}

TEST(PacketWriterTest, MultipleWriteStringCallsAppendSequentially) {
    PacketWriter writer(PacketType::ChatMessage, 1);
    ASSERT_TRUE(writer.WriteString("abc"));
    ASSERT_TRUE(writer.WriteString("de"));

    const std::vector<Nexus::byte> frame = writer.Build();

    Nexus::usize offset = HEADER_SIZE;
    Nexus::uint32 firstLength = 0;
    ASSERT_TRUE(Wire::Read(frame, offset, firstLength));
    EXPECT_EQ(firstLength, 3u);
    offset += firstLength;

    Nexus::uint32 secondLength = 0;
    ASSERT_TRUE(Wire::Read(frame, offset, secondLength));
    EXPECT_EQ(secondLength, 2u);
}

TEST(PacketWriterTest, BuildCanOnlyBeCalledOnce) {
    PacketWriter writer(PacketType::ChatMessage, 1);
    writer.Write(Nexus::uint32{1});

    const std::vector<Nexus::byte> first = writer.Build();
    EXPECT_FALSE(first.empty());

    const std::vector<Nexus::byte> second = writer.Build();
    EXPECT_TRUE(second.empty());
}

TEST(PacketWriterTest, DataReflectsBufferBeforeBuild) {
    PacketWriter writer(PacketType::ChatMessage, 1);
    writer.Write(Nexus::uint32{42});

    // Before Build(), Data() includes the still-unfilled 10-Nexus::byte header plus
    // the payload written so far.
    EXPECT_EQ(writer.Data().size(), HEADER_SIZE + sizeof(Nexus::uint32));
}

TEST(PacketWriterTest, IsMoveOnly) {
    static_assert(!std::is_copy_constructible_v<PacketWriter>);
    static_assert(!std::is_copy_assignable_v<PacketWriter>);
    static_assert(std::is_move_constructible_v<PacketWriter>);
    static_assert(std::is_move_assignable_v<PacketWriter>);

    SUCCEED();
}

TEST(PacketWriterTest, DifferentPacketTypesEncodeDifferentTypeValues) {
    PacketWriter helloWriter(PacketType::ClientHello, 1);
    PacketWriter chatWriter(PacketType::ChatMessage, 1);

    const std::vector<Nexus::byte> helloFrame = helloWriter.Build();
    const std::vector<Nexus::byte> chatFrame = chatWriter.Build();

    Nexus::usize helloOffset = sizeof(Nexus::uint32);
    Nexus::usize chatOffset = sizeof(Nexus::uint32);
    Nexus::uint16 helloType = 0;
    Nexus::uint16 chatType = 0;

    ASSERT_TRUE(Wire::Read(helloFrame, helloOffset, helloType));
    ASSERT_TRUE(Wire::Read(chatFrame, chatOffset, chatType));

    EXPECT_NE(helloType, chatType);
    EXPECT_EQ(helloType, static_cast<Nexus::uint16>(PacketType::ClientHello));
    EXPECT_EQ(chatType, static_cast<Nexus::uint16>(PacketType::ChatMessage));
}
