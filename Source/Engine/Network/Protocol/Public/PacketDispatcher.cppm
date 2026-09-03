// SPDX-License-Identifier: MIT

module;

#include <Platform/DLL/Export.hpp>

export module NE.Engine.Network.Protocol.PacketDispatcher;

import NE.Engine.Network.Packet;

import NE.Engine.Core.Types;

import std;

export namespace Nexus::Network {
    class NEXUS_API PacketDispatcher {
    public:
        using Handler = std::function<void(Packet&)>;

        void Register(PacketType type, Handler handler);

        void Unregister(PacketType type);

        void Clear();

        [[nodiscard]] bool HasHandler(PacketType type) const;

        void Dispatch(std::span<const byte> frame) const;

        void Dispatch(const std::vector<byte>& frame) const;

    private:
        std::unordered_map<PacketType, Handler> m_handlers;
    };
} // namespace Nexus::Network
