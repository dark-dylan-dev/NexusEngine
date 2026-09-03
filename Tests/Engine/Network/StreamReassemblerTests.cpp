// SPDX-License-Identifier: MIT

#include <gtest/gtest.h>

import NE.Engine.Network.Packet;
import NE.Engine.Network.PacketWriter;
import NE.Engine.Network.TCP.StreamReassembler;

import NE.Engine.Core.Types;

import std;

using namespace Nexus::Network;

namespace {
    class StreamReassemblerTest : public ::testing::Test {
    protected:
        StreamReassembler m_reassembler;
    };

    std::vector<Nexus::byte> BuildFrame(PacketType type, Nexus::uint32 sequence, Nexus::uint32 payloadValue) {
        PacketWriter writer(type, sequence);
        writer.Write(payloadValue);
        return writer.Build();
    }
} // namespace

TEST_F(StreamReassemblerTest, TryExtractReturnsNulloptWhenEmpty) {
    EXPECT_FALSE(m_reassembler.TryExtract().has_value());
}

TEST_F(StreamReassemblerTest, TryExtractReturnsNulloptForPartialHeader) {
    const auto frame = BuildFrame(PacketType::ChatMessage, 1, 42);
    m_reassembler.Feed(std::span(frame).first(3)); // fewer than HEADER_SIZE Bytes

    EXPECT_FALSE(m_reassembler.TryExtract().has_value());
}

TEST_F(StreamReassemblerTest, TryExtractReturnsNulloptForPartialPayload) {
    const auto frame = BuildFrame(PacketType::ChatMessage, 1, 42);
    // Feed the full header but only part of the payload.
    m_reassembler.Feed(std::span(frame).first(HEADER_SIZE + 1));

    EXPECT_FALSE(m_reassembler.TryExtract().has_value());
}

TEST_F(StreamReassemblerTest, ExtractsExactSingleFrame) {
    const auto frame = BuildFrame(PacketType::ChatMessage, 1, 42);
    m_reassembler.Feed(frame);

    const auto extracted = m_reassembler.TryExtract();
    ASSERT_TRUE(extracted.has_value());
    EXPECT_EQ(*extracted, frame);

    EXPECT_FALSE(m_reassembler.TryExtract().has_value());
}

TEST_F(StreamReassemblerTest, ExtractsMultipleFramesFedTogether) {
    const auto first = BuildFrame(PacketType::ChatMessage, 1, 1);
    const auto second = BuildFrame(PacketType::PlayerInput, 2, 2);

    std::vector<Nexus::byte> combined;
    combined.insert(combined.end(), first.begin(), first.end());
    combined.insert(combined.end(), second.begin(), second.end());
    m_reassembler.Feed(combined);

    const auto firstOut = m_reassembler.TryExtract();
    ASSERT_TRUE(firstOut.has_value());
    EXPECT_EQ(*firstOut, first);

    const auto secondOut = m_reassembler.TryExtract();
    ASSERT_TRUE(secondOut.has_value());
    EXPECT_EQ(*secondOut, second);

    EXPECT_FALSE(m_reassembler.TryExtract().has_value());
}

TEST_F(StreamReassemblerTest, ExtractsFrameSplitAcrossMultipleFeeds) {
    const auto frame = BuildFrame(PacketType::ChatMessage, 1, 42);

    // Feed one Nexus::byte at a time to simulate a slow/fragmented TCP stream.
    for (const Nexus::byte b : frame) {
        m_reassembler.Feed(std::span(&b, 1));
    }

    const auto extracted = m_reassembler.TryExtract();
    ASSERT_TRUE(extracted.has_value());
    EXPECT_EQ(*extracted, frame);
}

TEST_F(StreamReassemblerTest, FeedIgnoresEmptySpan) {
    m_reassembler.Feed(std::span<const Nexus::byte>{});
    EXPECT_FALSE(m_reassembler.HasInvalidData());
    EXPECT_FALSE(m_reassembler.TryExtract().has_value());
}

TEST_F(StreamReassemblerTest, HasInvalidDataFalseInitially) {
    EXPECT_FALSE(m_reassembler.HasInvalidData());
}

TEST_F(StreamReassemblerTest, OversizedPayloadMarksInvalid) {
    // Hand-build a frame header claiming a payload larger than MAX_PAYLOAD_SIZE.
    std::vector<Nexus::byte> frame(HEADER_SIZE, Nexus::byte{0});
    Nexus::usize offset = 0;
    const auto oversizedPayload = Wire::HostToBigEndian(MAX_PAYLOAD_SIZE + 1);
    std::memcpy(frame.data() + offset, &oversizedPayload, sizeof(oversizedPayload));

    m_reassembler.Feed(frame);
    EXPECT_FALSE(m_reassembler.TryExtract().has_value());
    EXPECT_TRUE(m_reassembler.HasInvalidData());
}

TEST_F(StreamReassemblerTest, FeedingMoreThanCapacityMarksInvalid) {
    const std::vector<Nexus::byte> tooMuch(StreamReassembler::MAX_BUFFERED_BYTES + 1, Nexus::byte{0});
    m_reassembler.Feed(tooMuch);

    EXPECT_TRUE(m_reassembler.HasInvalidData());
}

TEST_F(StreamReassemblerTest, FeedAfterInvalidIsNoOp) {
    const std::vector<Nexus::byte> tooMuch(StreamReassembler::MAX_BUFFERED_BYTES + 1, Nexus::byte{0});
    m_reassembler.Feed(tooMuch);
    ASSERT_TRUE(m_reassembler.HasInvalidData());

    const auto frame = BuildFrame(PacketType::ChatMessage, 1, 42);
    m_reassembler.Feed(frame);

    EXPECT_TRUE(m_reassembler.HasInvalidData());
    EXPECT_FALSE(m_reassembler.TryExtract().has_value());
}

TEST_F(StreamReassemblerTest, ResetClearsInvalidState) {
    const std::vector<Nexus::byte> tooMuch(StreamReassembler::MAX_BUFFERED_BYTES + 1, Nexus::byte{0});
    m_reassembler.Feed(tooMuch);
    ASSERT_TRUE(m_reassembler.HasInvalidData());

    m_reassembler.Reset();
    EXPECT_FALSE(m_reassembler.HasInvalidData());

    const auto frame = BuildFrame(PacketType::ChatMessage, 1, 42);
    m_reassembler.Feed(frame);
    const auto extracted = m_reassembler.TryExtract();
    ASSERT_TRUE(extracted.has_value());
    EXPECT_EQ(*extracted, frame);
}

TEST_F(StreamReassemblerTest, ResetClearsBufferedPartialFrame) {
    const auto frame = BuildFrame(PacketType::ChatMessage, 1, 42);
    m_reassembler.Feed(std::span(frame).first(HEADER_SIZE)); // header only, no payload yet

    m_reassembler.Reset();

    m_reassembler.Feed(std::span(frame).subspan(HEADER_SIZE));
    EXPECT_FALSE(m_reassembler.TryExtract().has_value());
}

TEST_F(StreamReassemblerTest, PayloadContentIsPreservedThroughExtraction) {
    const auto frame = BuildFrame(PacketType::PlayerState, 5, 0xCAFEBABE);
    m_reassembler.Feed(frame);

    const auto extracted = m_reassembler.TryExtract();
    ASSERT_TRUE(extracted.has_value());

    std::vector<Nexus::byte> payload(extracted->begin() + static_cast<Nexus::isize>(HEADER_SIZE), extracted->end());
    Packet reader(payload);
    Nexus::uint32 value = 0;
    ASSERT_TRUE(reader.Read(value));
    EXPECT_EQ(value, 0xCAFEBABEu);
}
