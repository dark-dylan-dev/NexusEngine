// SPDX-License-Identifier: MIT

export module NE.Engine.ECS.World;

import NE.Engine.Core.Types;
import NE.Engine.ECS.Component;
import NE.Engine.ECS.Entity;
import NE.Engine.ECS.Registry;

import std;

export namespace Nexus::ECS {
    class World {
    public:
        Entity CreateEntity();
        void DestroyEntity(Entity entity);

        bool IsAlive(Entity entity) const;

        void Clear();

        template <typename... Components>
        void Reserve(usize capacity) {
            (m_registry.Pool<Components>().Reserve(capacity), ...);
        }

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
            const auto* pool = m_registry.TryPool<T>();
            return pool && pool->Contains(entity);
        }

        template <Component T>
        T& Get(Entity entity) {
            return m_registry.Pool<T>().Get(entity);
        }

        template <Component T>
        const T& Get(Entity entity) const {
            return m_registry.Pool<T>().Get(entity);
        }

        template <Component T>
        T* TryGet(Entity entity) {
            auto* pool = m_registry.TryPool<T>();
            return pool ? pool->TryGet(entity) : nullptr;
        }

        template <Component T>
        const T* TryGet(Entity entity) const {
            const auto* pool = m_registry.TryPool<T>();
            return pool ? pool->TryGet(entity) : nullptr;
        }

        template <Component... Components>
        std::vector<Entity> Query() {
            return m_registry.Query<Components...>();
        }

        template <Component... Components>
        std::vector<Entity> QueryAny() {
            return m_registry.QueryAny<Components...>();
        }

        template <Component... Components>
        Registry::QueryView<Components...> View() {
            return m_registry.View<Components...>();
        }

    private:
        uint32 m_nextIndex = 1;
        std::vector<uint32> m_freeIndices;
        std::vector<uint32> m_generations;
        std::vector<uint8> m_alive;
        Registry m_registry;
    };
} // namespace Nexus::ECS
