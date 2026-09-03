// SPDX-License-Identifier: MIT

module;

#include <Platform/DLL/Export.hpp>

export module NE.Engine.Network.Common.NetworkAddress;

import NE.Engine.Core.Types;

import std;

export namespace Nexus::Network {
    class NEXUS_API NetworkAddress {
    public:
        NetworkAddress() = default;

        NetworkAddress(std::string host, uint16 port) : m_host(std::move(host)), m_port(port) {}

        [[nodiscard]] const std::string& Host() const noexcept;

        [[nodiscard]] uint16 Port() const noexcept;

        [[nodiscard]] bool IsLocal() const noexcept;

        [[nodiscard]] bool operator==(const NetworkAddress& other) const noexcept;

    private:
        std::string m_host;
        uint16 m_port = 0;
    };
} // namespace Nexus::Network

template <>
struct NEXUS_API std::hash<Nexus::Network::NetworkAddress>;

#include "NetworkAddress.inl"
