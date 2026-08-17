// SPDX-License-Identifier: MIT

export module NE.Engine.ECS.Registry;

import NE.Engine.Core.Types;
import NE.Engine.ECS.Component;
import NE.Engine.ECS.ComponentPool;
import NE.Engine.ECS.Entity;

import std;

export namespace Nexus::ECS {
    using ComponentId = uint32;

    inline ComponentId NextComponentId() {
        static ComponentId id = 0;
        return id++;
    }

    template <Component T>
    ComponentId ComponentTypeId() {
        static const ComponentId id = NextComponentId();
        return id;
    }

    class Registry {
    public:
        template <Component T>
        ComponentPool<T>& Pool() {
            const auto id = ComponentTypeId<T>();

            if (id >= m_pools.size())
                m_pools.resize(id + 1);

            if (!m_pools[id])
                m_pools[id] = std::make_unique<ComponentPool<T>>();

            return static_cast<ComponentPool<T>&>(*m_pools[id]);
        }

        template <Component T>
        const ComponentPool<T>& Pool() const {
            return static_cast<const ComponentPool<T>&>(*m_pools.at(ComponentTypeId<T>()));
        }

        template <Component T>
        bool HasPool() const {
            const auto id = ComponentTypeId<T>();

            return id < m_pools.size() && m_pools[id];
        }

        template <Component... Components>
        std::vector<Entity> Query() {
            if constexpr (sizeof...(Components) == 0)
                return {};

            if (!(HasPool<Components>() && ...))
                return {};

            const std::array<IComponentPool*, sizeof...(Components)> pools{&Pool<Components>()...};

            auto* smallest = pools.front();

            for (auto* pool : pools) {
                if (pool->Entities().size() < smallest->Entities().size())
                    smallest = pool;
            }

            std::vector<Entity> result;
            result.reserve(smallest->Entities().size());

            for (const auto entity : smallest->Entities()) {
                if ((Pool<Components>().Contains(entity) && ...))
                    result.push_back(entity);
            }

            return result;
        }

        template <Component... Components>
        std::vector<Entity> QueryAny() {
            if constexpr (sizeof...(Components) == 0)
                return {};

            std::size_t maxIndex = 0;

            auto findMax = [&]<Component T> {
                if (!HasPool<T>())
                    return;

                for (const auto entity : Pool<T>().Entities())
                    maxIndex = std::max(maxIndex, static_cast<std::size_t>(entity.index));
            };

            (findMax.template operator()<Components>(), ...);

            std::vector<Entity> result;
            result.reserve(maxIndex + 1);

            std::vector<std::uint8_t> seen(maxIndex + 1);

            auto add = [&]<Component T> {
                if (!HasPool<T>())
                    return;

                for (const auto entity : Pool<T>().Entities()) {
                    if (!seen[entity.index]) {
                        seen[entity.index] = 1;
                        result.push_back(entity);
                    }
                }
            };

            (add.template operator()<Components>(), ...);

            return result;
        }

        void RemoveEntity(Entity entity);

    private:
        std::vector<std::unique_ptr<IComponentPool>> m_pools;
    };
} // namespace Nexus::ECS
