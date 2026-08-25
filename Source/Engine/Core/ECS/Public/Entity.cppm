// SPDX-License-Identifier: MIT

module;

#include <Platform/DLL/Export.hpp>

export module NE.Engine.ECS.Entity;

import NE.Engine.Core.Types;

export namespace Nexus::ECS {
    struct NEXUS_API Entity {
        uint32 index;
        uint32 generation;
        bool operator==(const Entity& b) const {
            return index == b.index && generation == b.generation;
        }
    };

    NEXUS_API constexpr Entity NullEntity{};
} // namespace Nexus::ECS
