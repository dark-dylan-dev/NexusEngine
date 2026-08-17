// SPDX-License-Identifier: MIT

module NE.Engine.ECS.World;

import std;

namespace Nexus::ECS {
    Entity World::CreateEntity() {
        return {.index = m_nextEntity.index++, .generation = 0};
    }

    void World::DestroyEntity(Entity entity) {
        m_registry.RemoveEntity(entity);
    }
} // namespace Nexus::ECS
