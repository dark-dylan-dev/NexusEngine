// SPDX-License-Identifier: MIT

export module NE.Engine.ECS.SparseSet;

import NE.Engine.Core.Types;
import NE.Engine.ECS.Entity;

import std;

export namespace Nexus::ECS {
    template <typename T>
    class SparseSet {
    public:
        template <typename... Args>
        T& Emplace(Entity entity, Args&&... args) {
            if (Contains(entity))
                return Get(entity);

            const auto index = m_dense.size();

            if (entity.index >= m_sparse.size())
                m_sparse.resize(entity.index + 1, 0);

            m_denseEntities.push_back(entity);
            m_dense.emplace_back(std::forward<Args>(args)...);

            m_sparse[entity.index] = index + 1;

            return m_dense.back();
        }

        void Remove(Entity entity) {
            if (!Contains(entity))
                return;

            const auto index = m_sparse[entity.index] - 1;
            const auto last = m_dense.size() - 1;

            if (index != last) {
                m_denseEntities[index] = m_denseEntities[last];

                m_dense[index] = std::move(m_dense[last]);

                const Entity movedEntity = m_denseEntities[index];

                m_sparse[movedEntity.index] = index + 1;
            }

            m_denseEntities.pop_back();
            m_dense.pop_back();

            m_sparse[entity.index] = 0;
        }

        bool Contains(Entity entity) const {
            return entity.index < m_sparse.size() && m_sparse[entity.index] != 0;
        }

        T& Get(Entity entity) {
            if (!Contains(entity))
                throw std::out_of_range("SparseSet::Get: entity not present");

            return m_dense[m_sparse[entity.index] - 1];
        }

        const T& Get(Entity entity) const {
            if (!Contains(entity))
                throw std::out_of_range("SparseSet::Get: entity not present");

            return m_dense[m_sparse[entity.index] - 1];
        }

        T* TryGet(Entity entity) {
            if (!Contains(entity))
                return nullptr;

            return &m_dense[m_sparse[entity.index] - 1];
        }

        const T* TryGet(Entity entity) const {
            if (!Contains(entity))
                return nullptr;

            return &m_dense[m_sparse[entity.index] - 1];
        }

        std::span<const Entity> Entities() const {
            return m_denseEntities;
        }

        std::span<T> Values() {
            return m_dense;
        }

        std::span<const T> Values() const {
            return m_dense;
        }

    private:
        std::vector<Entity> m_denseEntities;
        std::vector<T> m_dense;
        std::vector<uint32> m_sparse;
    };
} // namespace Nexus::ECS
