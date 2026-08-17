// SPDX-License-Identifier: MIT

export module NE.Engine.ECS.Component;

import std;

export namespace Nexus::ECS {
    template <typename T>
    concept Component = std::is_trivially_destructible_v<T>;
} // namespace Nexus::ECS
