// SPDX-License-Identifier: MIT

/// @file Entity.cppm
/// @brief Module NE.Engine.ECS.Entity: the generational handle used to reference entities.

module;

#include <Platform/DLL/Export.hpp>

export module NE.Engine.ECS.Entity;

import NE.Engine.Core.Types;

export namespace Nexus::ECS {
    /// @brief Generational handle identifying an entity inside a World.
    ///
    /// An Entity is a small value type (index + generation). The index addresses a slot in the
    /// World and in the sparse array of every ComponentPool; the generation is incremented each
    /// time a slot is recycled, so handles that outlived their entity are detected by
    /// World::IsAlive() instead of silently aliasing a newer entity.
    ///
    /// @note Index 0 is reserved: World never hands it out (see NullEntity).
    /// @thread_safety Plain value type: copying and comparing from any thread is safe.
    /// @see World::CreateEntity, World::IsAlive, NullEntity
    struct NEXUS_API Entity {
        /// @brief Slot number inside the World (0 is reserved for NullEntity).
        uint32 index;
        /// @brief Number of times the slot was recycled; must match the World's current generation for the handle to be alive.
        uint32 generation;
        bool operator==(const Entity& b) const {
            return index == b.index && generation == b.generation;
        }
    };

    /// @brief The "no entity" handle ({0, 0}).
    ///
    /// Index 0 is never allocated by World, so World::IsAlive(NullEntity) is always false.
    /// Use it as the default value for optional entity references.
    NEXUS_API constexpr Entity NullEntity{};
} // namespace Nexus::ECS
