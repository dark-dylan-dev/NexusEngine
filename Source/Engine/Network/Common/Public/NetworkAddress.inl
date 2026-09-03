// SPDX-License-Identifier: MIT

#pragma once

template <>
struct NEXUS_API std::hash<Nexus::Network::NetworkAddress> {
    Nexus::usize operator()(const Nexus::Network::NetworkAddress& address) const noexcept {
        const Nexus::usize h1 = std::hash<std::string>{}(address.Host());
        const Nexus::usize h2 = std::hash<Nexus::uint16>{}(address.Port());

        // 64-bit mix inspired by the standard hash-combine pattern.
        static constexpr auto PHI = 0x9e3779b97f4a7c15ULL;
        return h1 ^ (h2 + static_cast<Nexus::usize>(PHI) + (h1 << 6) + (h1 >> 2));
    }
};
