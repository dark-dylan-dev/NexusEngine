// SPDX-License-Identifier: MIT

export module NE.Engine.ECS.ComponentPool;

import NE.Engine.ECS.Component;
import NE.Engine.ECS.Entity;
import NE.Engine.ECS.SparseSet;

import std;

export namespace Nexus::ECS {
    class IComponentPool {
    public:
        virtual ~IComponentPool() = default;

        virtual void Remove(Entity entity) = 0;
        virtual bool Contains(Entity entity) const = 0;
        virtual std::span<const Entity> Entities() const = 0;
    };

    template <Component T>
    class ComponentPool final : public IComponentPool {
    public:
        template <typename... Args>
        T& Emplace(Entity entity, Args&&... args) {
            return m_storage.Emplace(entity, T(std::forward<Args>(args)...));
        }

        void Remove(Entity entity) override {
            m_storage.Remove(entity);
        }

        bool Contains(Entity entity) const override {
            return m_storage.Contains(entity);
        }

        T& Get(Entity entity) {
            return m_storage.Get(entity);
        }

        std::span<const Entity> Entities() const override {
            return m_storage.Entities();
        }

    private:
        SparseSet<T> m_storage;
    };
} // namespace Nexus::ECS
