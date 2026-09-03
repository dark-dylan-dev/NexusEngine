// SPDX-License-Identifier: MIT

#pragma once

namespace Nexus::Network {
    namespace Wire {
        template <std::integral T>
        [[nodiscard]] constexpr T HostToBigEndian(T value) noexcept {
            if constexpr (std::endian::native == std::endian::little && sizeof(T) > 1) {
                return std::byteswap(value);
            } else {
                return value;
            }
        }

        template <std::integral T>
        [[nodiscard]] constexpr T BigEndianToHost(T value) noexcept {
            return HostToBigEndian(value);
        }

        template <std::integral T>
        void Append(std::vector<byte>& buffer, T value) {
            const T wireValue = HostToBigEndian(value);
            const auto* bytes = reinterpret_cast<const byte*>(&wireValue);
            buffer.insert(buffer.end(), bytes, bytes + sizeof(T));
        }

        template <std::integral T>
        [[nodiscard]] bool Read(std::span<const byte> data, usize& offset, T& out) {
            if (offset > data.size() || sizeof(T) > data.size() - offset) {
                return false;
            }

            T wireValue{};
            std::memcpy(&wireValue, data.data() + offset, sizeof(T));
            offset += sizeof(T);
            out = BigEndianToHost(wireValue);
            return true;
        }
    } // namespace Wire

    template <typename T>
    void Packet::Write(const T& value) {
        static_assert(std::is_trivially_copyable_v<T>);

        if constexpr (std::integral<T>) {
            Wire::Append(m_storage, value);
        } else {
            const auto* bytes = reinterpret_cast<const byte*>(&value);
            m_storage.insert(m_storage.end(), bytes, bytes + sizeof(T));
        }
    }

    template <typename T>
    [[nodiscard]] bool Packet::Read(T& out) {
        static_assert(std::is_trivially_copyable_v<T>);

        if constexpr (std::integral<T>) {
            return Wire::Read(m_storage, m_readPos, out);
        } else {
            if (m_readPos > m_storage.size() || sizeof(T) > m_storage.size() - m_readPos) {
                return false;
            }
            std::memcpy(&out, m_storage.data() + m_readPos, sizeof(T));
            m_readPos += sizeof(T);
            return true;
        }
    }
} // namespace Nexus::Network
