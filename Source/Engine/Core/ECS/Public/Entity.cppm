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
        static constexpr uint32 PHI = 0x9e3779b9;
        usize operator()(const Entity& e) const noexcept {
            return e.index ^ (static_cast<usize>(e.generation) * PHI);
        }
    };

    constexpr Entity NullEntity{};
} // namespace Nexus::ECS
