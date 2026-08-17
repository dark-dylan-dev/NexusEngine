// SPDX-License-Identifier: MIT

export module NE.Engine.ECS.Entity;

import NE.Engine.Core.Types;

export namespace Nexus::ECS {
    struct Entity {
        uint32 index;
        uint32 generation;

        constexpr bool operator==(const Entity&) const = default;
    };

    struct EntityHash {
        usize operator()(Entity entity) const noexcept {
            return (static_cast<usize>(entity.index) << 32) ^ entity.generation;
        }
    };

    constexpr Entity NullEntity{};
} // namespace Nexus::ECS
