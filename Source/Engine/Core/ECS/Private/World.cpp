// SPDX-License-Identifier: MIT

module NE.Engine.ECS.World;

import std;

namespace Nexus::ECS {
    Entity World::CreateEntity() {
        uint32 index;

        if (!m_freeIndices.empty()) {
            index = m_freeIndices.back();
            m_freeIndices.pop_back();
        } else {
            index = m_nextIndex++;

            if (index >= m_generations.size()) {
                m_generations.resize(index + 1, 0);
                m_alive.resize(index + 1, false);
            }
        }

        m_alive[index] = true;

        return {.index = index, .generation = m_generations[index]};
    }

    void World::DestroyEntity(Entity entity) {
        if (!IsAlive(entity))
            return;

        m_registry.RemoveEntity(entity);

        m_alive[entity.index] = false;
        ++m_generations[entity.index];
        m_freeIndices.push_back(entity.index);
    }
} // namespace Nexus::ECS
