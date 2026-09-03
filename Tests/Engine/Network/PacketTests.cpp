// SPDX-License-Identifier: MIT

#include <gtest/gtest.h>

import NE.Engine.Network.Packet;

import NE.Engine.Core.Types;

import std;

using namespace Nexus::Network;

namespace {
    class PacketTest : public ::testing::Test {
    protected:
        std::vector<Nexus::byte> m_storage;
    };
} // namespace

// ---- Wire -------------------------------------------------------------

TEST(WireTest, HostToBigEndianRoundTripsThroughBigEndianToHost) {
    constexpr Nexus::uint32 value = 0x12345678u;
    const Nexus::uint32 wire = Wire::HostToBigEndian(value);
    const Nexus::uint32 back = Wire::BigEndianToHost(wire);

    EXPECT_EQ(back, value);
}

TEST(WireTest, HostToBigEndianProducesBigEndianByteOrder) {
    constexpr Nexus::uint32 value = 0x12345678u;
    const Nexus::uint32 wire = Wire::HostToBigEndian(value);

    std::array<Nexus::byte, sizeof(Nexus::uint32)> bytes{};
    std::memcpy(bytes.data(), &wire, sizeof(wire));

    // Big-endian: most significant Nexus::byte first, regardless of host endianness.
    EXPECT_EQ(static_cast<Nexus::uint8>(bytes[0]), 0x12);
    EXPECT_EQ(static_cast<Nexus::uint8>(bytes[1]), 0x34);
    EXPECT_EQ(static_cast<Nexus::uint8>(bytes[2]), 0x56);
    EXPECT_EQ(static_cast<Nexus::uint8>(bytes[3]), 0x78);
}

TEST(WireTest, SingleByteTypeIsUnaffectedByByteOrder) {
    constexpr Nexus::uint8 value = 0xAB;
    EXPECT_EQ(Wire::HostToBigEndian(value), value);
}

TEST(WireTest, AppendThenReadRoundTrips) {
    std::vector<Nexus::byte> buffer;
    Wire::Append(buffer, Nexus::uint32{0xDEADBEEF});

    Nexus::usize offset = 0;
    Nexus::uint32 out = 0;
    ASSERT_TRUE(Wire::Read(buffer, offset, out));
    EXPECT_EQ(out, 0xDEADBEEFu);
    EXPECT_EQ(offset, sizeof(Nexus::uint32));
}

TEST(WireTest, ReadFailsWhenBufferTooShort) {
    const std::vector<Nexus::byte> buffer(2, Nexus::byte{0});
    Nexus::usize offset = 0;
    Nexus::uint32 out = 0;

    EXPECT_FALSE(Wire::Read(buffer, offset, out));
}

TEST(WireTest, ReadFailsWhenOffsetPastEnd) {
    const std::vector<Nexus::byte> buffer(4, Nexus::byte{0});
    Nexus::usize offset = 10;
    Nexus::uint32 out = 0;

    EXPECT_FALSE(Wire::Read(buffer, offset, out));
}

TEST(WireTest, MultipleAppendsAreReadBackInOrder) {
    std::vector<Nexus::byte> buffer;
    Wire::Append(buffer, Nexus::uint16{111});
    Wire::Append(buffer, Nexus::uint32{222});
    Wire::Append(buffer, Nexus::uint8{33});

    Nexus::usize offset = 0;
    Nexus::uint16 a = 0;
    Nexus::uint32 b = 0;
    Nexus::uint8 c = 0;

    ASSERT_TRUE(Wire::Read(buffer, offset, a));
    ASSERT_TRUE(Wire::Read(buffer, offset, b));
    ASSERT_TRUE(Wire::Read(buffer, offset, c));

    EXPECT_EQ(a, 111);
    EXPECT_EQ(b, 222u);
    EXPECT_EQ(c, 33);
}

// ---- Packet -------------------------------------------------------------

TEST_F(PacketTest, WriteThenReadIntegralRoundTrips) {
    Packet writer(m_storage);
    writer.Write(Nexus::uint32{424242});

    Packet reader(m_storage);
    Nexus::uint32 out = 0;
    ASSERT_TRUE(reader.Read(out));
    EXPECT_EQ(out, 424242u);
}

TEST_F(PacketTest, WriteThenReadBoolRoundTrips) {
    Packet writer(m_storage);
    writer.Write(true);
    writer.Write(false);

    Packet reader(m_storage);
    bool first = false;
    bool second = true;
    ASSERT_TRUE(reader.Read(first));
    ASSERT_TRUE(reader.Read(second));

    EXPECT_TRUE(first);
    EXPECT_FALSE(second);
}

TEST_F(PacketTest, WriteThenReadFloatRoundTrips) {
    Packet writer(m_storage);
    writer.Write(3.14159f);

    Packet reader(m_storage);
    Nexus::float32 out = 0.0f;
    ASSERT_TRUE(reader.Read(out));
    EXPECT_FLOAT_EQ(out, 3.14159f);
}

TEST_F(PacketTest, WriteThenReadStringRoundTrips) {
    Packet writer(m_storage);
    std::string text = "hello, packet";

    // WriteString helper that replicates the exact wire format ReadString expects
    // Nexus::uint32 length followed by the raw Nexus::bytes.
    writer.Write(static_cast<Nexus::uint32>(text.size()));
    m_storage.insert(m_storage.end(), reinterpret_cast<const Nexus::byte*>(text.data()),
                     reinterpret_cast<const Nexus::byte*>(text.data()) + text.size());

    Packet reader(m_storage);
    std::string out;
    ASSERT_TRUE(reader.ReadString(out));
    EXPECT_EQ(out, text);
}

TEST_F(PacketTest, ReadStringFailsWhenLengthExceedsRemainingBytes) {
    Packet writer(m_storage);
    writer.Write(Nexus::uint32{1000}); // claims 1000 Nexus::bytes follow, but none do

    Packet reader(m_storage);
    std::string out;
    EXPECT_FALSE(reader.ReadString(out));
}

TEST_F(PacketTest, ReadFailsPastEndOfBuffer) {
    Packet writer(m_storage);
    writer.Write(Nexus::uint8{1});

    Packet reader(m_storage);
    Nexus::uint8 first = 0;
    Nexus::uint8 second = 0;
    ASSERT_TRUE(reader.Read(first));
    EXPECT_FALSE(reader.Read(second));
}

TEST_F(PacketTest, MultipleFieldsRoundTripInOrder) {
    Packet writer(m_storage);
    writer.Write(Nexus::uint32{1});
    writer.Write(Nexus::uint16{2});
    writer.Write(true);
    writer.Write(4.5f);

    Packet reader(m_storage);
    Nexus::uint32 a = 0;
    Nexus::uint16 b = 0;
    bool c = false;
    Nexus::float32 d = 0.0f;

    ASSERT_TRUE(reader.Read(a));
    ASSERT_TRUE(reader.Read(b));
    ASSERT_TRUE(reader.Read(c));
    ASSERT_TRUE(reader.Read(d));

    EXPECT_EQ(a, 1u);
    EXPECT_EQ(b, 2);
    EXPECT_TRUE(c);
    EXPECT_FLOAT_EQ(d, 4.5f);
}

TEST_F(PacketTest, RemainingReflectsUnreadBytes) {
    Packet writer(m_storage);
    writer.Write(Nexus::uint32{1});
    writer.Write(Nexus::uint32{2});

    Packet reader(m_storage);
    EXPECT_EQ(reader.Remaining(), 2 * sizeof(Nexus::uint32));

    Nexus::uint32 value = 0;
    ASSERT_TRUE(reader.Read(value));
    EXPECT_EQ(reader.Remaining(), sizeof(Nexus::uint32));

    ASSERT_TRUE(reader.Read(value));
    EXPECT_EQ(reader.Remaining(), 0u);
}

TEST_F(PacketTest, ResetRewindsReadCursor) {
    Packet writer(m_storage);
    writer.Write(Nexus::uint32{99});

    Packet reader(m_storage);
    Nexus::uint32 first = 0;
    ASSERT_TRUE(reader.Read(first));
    EXPECT_EQ(reader.Remaining(), 0u);

    reader.Reset();
    EXPECT_EQ(reader.Remaining(), sizeof(Nexus::uint32));

    Nexus::uint32 second = 0;
    ASSERT_TRUE(reader.Read(second));
    EXPECT_EQ(second, 99u);
}

TEST_F(PacketTest, DataReflectsUnderlyingStorage) {
    Packet writer(m_storage);
    writer.Write(Nexus::uint8{7});

    EXPECT_EQ(writer.Data().size(), 1u);
    EXPECT_EQ(static_cast<Nexus::uint8>(writer.Data()[0]), 7);
}

TEST_F(PacketTest, EmptyStorageHasZeroRemaining) {
    Packet reader(m_storage);
    EXPECT_EQ(reader.Remaining(), 0u);
}

TEST_F(PacketTest, HeaderSizeConstantMatchesWireLayout) {
    // Nexus::uint32 (payload size) + Nexus::uint16 (type) + Nexus::uint32 (sequence) = 10 Nexus::bytes.
    EXPECT_EQ(HEADER_SIZE, 10u);
}
