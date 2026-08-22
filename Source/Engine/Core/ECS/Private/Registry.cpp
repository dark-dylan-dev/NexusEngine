// SPDX-License-Identifier: MIT

module NE.Engine.ECS.Registry;

namespace Nexus::ECS {
    void Registry::RemoveEntity(Entity entity) {
        for (const auto& pool : m_pools)
            if (pool)
                pool->Remove(entity);
    }

    void Registry::Clear() {
        for (const auto& pool : m_pools)
            if (pool)
                pool->Clear();
    }
} // namespace Nexus::ECS
