// SPDX-License-Identifier: MIT

export module NE.Engine.ECS.World;

import NE.Engine.ECS.Component;
import NE.Engine.ECS.Entity;
import NE.Engine.ECS.Registry;

import std;

export namespace Nexus::ECS {
    class World {
    public:
        Entity CreateEntity();
        void DestroyEntity(Entity entity);

        template <Component T, typename... Args>
        T& Add(Entity entity, Args&&... args) {
            return m_registry.Pool<T>().Emplace(entity, std::forward<Args>(args)...);
        }

        template <Component T>
        void Remove(Entity entity) {
            m_registry.Pool<T>().Remove(entity);
        }

        template <Component T>
        bool Has(Entity entity) const {
            return m_registry.Pool<T>().Contains(entity);
        }

        template <Component T>
        T& Get(Entity entity) {
            return m_registry.Pool<T>().Get(entity);
        }

        template <Component... Components>
        std::vector<Entity> Query() {
            return m_registry.Query<Components...>();
        }

        template <Component... Components>
        std::vector<Entity> QueryAny() {
            return m_registry.QueryAny<Components...>();
        }

    private:
        Entity m_nextEntity = {.index = 1, .generation = 0};
        Registry m_registry;
    };
} // namespace Nexus::ECS
