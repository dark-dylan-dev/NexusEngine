// SPDX-License-Identifier: MIT

module NE.Editor.Application;

import NexusEngine;

namespace Nexus {

    struct Vec3 {
        float x = 0.f, y = 0.f, z = 0.f;

        Vec3 operator+(const Vec3& rhs) const {
            return {x + rhs.x, y + rhs.y, z + rhs.z};
        }

        Vec3& operator+=(const Vec3& rhs) {
            x += rhs.x;
            y += rhs.y;
            z += rhs.z;
            return *this;
        }

        Vec3 operator*(float rhs) const {
            return {x * rhs, y * rhs, z * rhs};
        }
    };

    struct Transform {
        Vec3 position = {};
        Vec3 rotation = {};
        Vec3 scale = {};
    };

    struct Velocity {
        Vec3 velocity = {};
    };

    struct Camera {
        Vec3 position = {};
        Velocity velocity = {};
        float fovy = 60.f;
    };

    void Application::run() {
        Logger logger;
        logger.Log(LogLevel::Info, std::format("Hello {}", Config::engineName));
        ECS::World world;

        constexpr std::size_t entityCount = 100'000;
        constexpr float deltaTime = 1.0f / 60.0f;

        std::vector<ECS::Entity> entities;
        entities.reserve(entityCount);

        const auto start = std::chrono::steady_clock::now();

        for (std::size_t i = 0; i < entityCount; ++i) {
            const auto entity = world.CreateEntity();

            world.Add<Transform>(entity);
            world.Add<Velocity>(entity);

            if (i % 2 == 0)
                world.Add<Camera>(entity);

            entities.push_back(entity);
        }

        const auto query = world.Query<Transform, Velocity>();

        for (const auto entity : query) {
            auto& transform = world.Get<Transform>(entity);
            const auto& velocity = world.Get<Velocity>(entity);

            transform.position += velocity.velocity * deltaTime;
        }

        const auto end = std::chrono::steady_clock::now();

        logger.Log(LogLevel::Info,
                   std::format("ECS: {} entities, {} matching Transform + Velocity", entityCount, query.size()));

        logger.Log(LogLevel::Info,
                   std::format("ECS: benchmark took {} us",
                               std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()));
    }

} // namespace Nexus
