// SPDX-License-Identifier: MIT

export module NE.Engine.ECS.Entity;

import NE.Engine.Core.Types;

export namespace Nexus::ECS {
    struct Entity {
        uint32 index;
        uint32 generation;
        bool operator==(const Entity& b) const {
            return index == b.index && generation == b.generation;
        }
    };

    constexpr Entity NullEntity{};
} // namespace Nexus::ECS
