// SPDX-License-Identifier: MIT

/// @file ECS.cppm
/// @brief Umbrella module NE.Engine.Core.ECS: re-exports every ECS sub-module.
///
/// `import NE.Engine.Core.ECS;` gives access to CommandBuffer, Component, Entity, Registry,
/// Scheduler, ThreadPool and World. (SparseSet and ComponentPool are reachable through
/// Registry's interface but are not re-exported here.)

export module NE.Engine.Core.ECS;

export import NE.Engine.ECS.CommandBuffer;
export import NE.Engine.ECS.Component;
export import NE.Engine.ECS.Entity;
export import NE.Engine.ECS.Registry;
export import NE.Engine.ECS.Scheduler;
export import NE.Engine.ECS.ThreadPool;
export import NE.Engine.ECS.World;
