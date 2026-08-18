// SPDX-License-Identifier: MIT

export module NE.Engine.ECS.Entity;

import NE.Engine.Core.Types;

export namespace Nexus::ECS {
    struct Entity {
        uint32 index;
        uint32 generation;
    };

    constexpr Entity NullEntity{};
} // namespace Nexus::ECS
