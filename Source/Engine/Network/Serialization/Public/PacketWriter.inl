// SPDX-License-Identifier: MIT

#pragma once

namespace Nexus::Network {
    template <typename T>
    void PacketWriter::Write(const T& value) {
        static_assert(!std::is_pointer_v<T>);
        static_assert(std::is_trivially_copyable_v<T>);

        if constexpr (std::integral<T>) {
            Wire::Append(m_buffer, value);
        } else {
            const auto* bytes = reinterpret_cast<const byte*>(&value);
            m_buffer.insert(m_buffer.end(), bytes, bytes + sizeof(T));
        }
    }
} // namespace Nexus::Network
