// SPDX-License-Identifier: MIT

#include <gtest/gtest.h>

import NE.Engine.Network.Common.NetworkAddress;

import NE.Engine.Core.Types;

import std;

using Nexus::Network::NetworkAddress;

TEST(NetworkAddressTest, DefaultConstruction) {
    const NetworkAddress address;

    EXPECT_TRUE(address.Host().empty());
    EXPECT_EQ(address.Port(), 0);
}

TEST(NetworkAddressTest, ConstructionWithHostAndPort) {
    const NetworkAddress address("127.0.0.1", 8080);

    EXPECT_EQ(address.Host(), "127.0.0.1");
    EXPECT_EQ(address.Port(), 8080);
}

TEST(NetworkAddressTest, EqualityWhenHostAndPortMatch) {
    const NetworkAddress first("localhost", 27015);
    const NetworkAddress second("localhost", 27015);

    EXPECT_TRUE(first == second);
}

TEST(NetworkAddressTest, InequalityWhenHostDiffers) {
    const NetworkAddress first("localhost", 27015);
    const NetworkAddress second("example.com", 27015);

    EXPECT_FALSE(first == second);
}

TEST(NetworkAddressTest, InequalityWhenPortDiffers) {
    const NetworkAddress first("localhost", 27015);
    const NetworkAddress second("localhost", 27016);

    EXPECT_FALSE(first == second);
}

TEST(NetworkAddressTest, HashIsConsistentForEqualAddresses) {
    const NetworkAddress first("localhost", 27015);
    const NetworkAddress second("localhost", 27015);

    const std::hash<NetworkAddress> hasher;
    EXPECT_EQ(hasher(first), hasher(second));
}

TEST(NetworkAddressTest, HashDiffersForDifferentPorts) {
    const NetworkAddress first("localhost", 27015);
    const NetworkAddress second("localhost", 27016);

    const std::hash<NetworkAddress> hasher;

    EXPECT_NE(hasher(first), hasher(second));
}

TEST(NetworkAddressTest, UsableAsUnorderedMapKey) {
    std::unordered_map<NetworkAddress, int> map;
    map[NetworkAddress("127.0.0.1", 1000)] = 1;
    map[NetworkAddress("127.0.0.1", 2000)] = 2;

    EXPECT_EQ(map.at(NetworkAddress("127.0.0.1", 1000)), 1);
    EXPECT_EQ(map.at(NetworkAddress("127.0.0.1", 2000)), 2);
    EXPECT_EQ(map.size(), 2u);
}

TEST(NetworkAddressTest, PortAtMaxUint16) {
    const NetworkAddress address("host", 65535);
    EXPECT_EQ(address.Port(), 65535);
}

TEST(NetworkAddressTest, IsLocalWorks) {
    const NetworkAddress localhost("localhost", 27017);
    const NetworkAddress ipv4local("127.0.0.1", 27018);
    const NetworkAddress ipv4("128.0.0.1", 27019);
    EXPECT_TRUE(localhost.IsLocal());
    EXPECT_TRUE(ipv4local.IsLocal());
    EXPECT_FALSE(ipv4.IsLocal());
}
