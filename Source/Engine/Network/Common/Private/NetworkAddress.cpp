// SPDX-License-Identifier: MIT

module NE.Engine.Network.Common.NetworkAddress;

import NE.Engine.Core.Types;

import std;

namespace Nexus::Network {
    [[nodiscard]] const std::string& NetworkAddress::Host() const noexcept {
        return m_host;
    }

    [[nodiscard]] uint16 NetworkAddress::Port() const noexcept {
        return m_port;
    }

    [[nodiscard]] bool NetworkAddress::IsLocal() const noexcept {
        return m_host == "localhost" || m_host.starts_with("127.");
    }

    [[nodiscard]] bool NetworkAddress::operator==(const NetworkAddress& other) const noexcept {
        return m_port == other.m_port && m_host == other.m_host;
    }
} // namespace Nexus::Network
