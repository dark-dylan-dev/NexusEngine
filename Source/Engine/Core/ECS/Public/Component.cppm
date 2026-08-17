// SPDX-License-Identifier: MIT

export module NE.Engine.ECS.Component;

import std;

export namespace Nexus::ECS {
    template <typename T>
    concept Component = std::is_object_v<T> && std::is_move_constructible_v<T> && std::is_move_assignable_v<T> &&
                        std::is_destructible_v<T>;
} // namespace Nexus::ECS
