// SPDX-License-Identifier: MIT

module;

#include <Platform/DLL/Export.hpp>

export module NE.Engine.Network.Packet;

import NE.Engine.Core.Types;

import std;

export namespace Nexus::Network {
    // Placeholder types, changes guaranteed at some point
    enum class PacketType : uint16 {
        ClientHello,
        ServerHello,
        PlayerInput,
        PlayerState,
        SpawnEntity,
        DestroyEntity,
        ChatMessage,
    };

    struct NEXUS_API PacketHeader {
        uint32 size = 0;
        PacketType type = {};
        uint32 sequence = 0;
    };

    inline constexpr usize HEADER_SIZE = sizeof(uint32) + sizeof(PacketType) + sizeof(uint32);

    inline constexpr uint32 MAX_PAYLOAD_SIZE = 1u << 20; // 1 MiB
    inline constexpr usize MAX_FRAME_SIZE = HEADER_SIZE + static_cast<usize>(MAX_PAYLOAD_SIZE);

    static_assert(HEADER_SIZE == 10);

    /// Wire values are always sent big-endian.
    namespace Wire {
        template <std::integral T>
        [[nodiscard]] constexpr T HostToBigEndian(T value) noexcept;

        template <std::integral T>
        [[nodiscard]] constexpr T BigEndianToHost(T value) noexcept;

        template <std::integral T>
        void Append(std::vector<byte>& buffer, T value);

        template <std::integral T>
        [[nodiscard]] bool Read(std::span<const byte> data, usize& offset, T& out);
    } // namespace Wire

    /// A read/write cursor over a byte buffer. Integral types go through Wire
    /// (big-endian on the wire); other trivially copyable types are copied raw.
    class NEXUS_API Packet {
    public:
        explicit Packet(std::vector<byte>& storage) noexcept : m_storage(storage) {}

        template <typename T>
        void Write(const T& value);

        template <typename T>
        [[nodiscard]] bool Read(T& out);

        [[nodiscard]] bool ReadString(std::string& out);

        [[nodiscard]] std::span<const byte> Data() const noexcept;

        [[nodiscard]] usize Remaining() const noexcept;

        void Reset() noexcept;

    private:
        std::vector<byte>& m_storage;
        usize m_readPos = 0;
    };
} // namespace Nexus::Network

#include "Packet.inl"
