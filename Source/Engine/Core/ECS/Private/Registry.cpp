// SPDX-License-Identifier: MIT

/// @file Registry.cpp
/// @brief Implementation unit of NE.Engine.ECS.Registry (documented in Registry.cppm).

module NE.Engine.ECS.Registry;

import std;

namespace Nexus::ECS {
    ComponentId NextComponentId() {
        static std::atomic<ComponentId> id = 0;
        return id.fetch_add(1, std::memory_order_relaxed);
    }

    void Registry::RemoveEntity(Entity entity) {
        // Pools are created lazily, so the table may contain empty slots.
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
