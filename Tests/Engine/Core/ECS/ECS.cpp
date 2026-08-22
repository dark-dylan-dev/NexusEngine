// SPDX-License-Identifier: MIT

#include <gtest/gtest.h>

import NE.Engine.Core.ECS;
import NE.Engine.Core.Types;

import std;

using namespace Nexus;
using namespace Nexus::ECS;

namespace {
    bool Contains(const std::vector<Entity>& entities, Entity target) {
        return std::ranges::any_of(entities, [&](Entity e) { return e == target; });
    }

    class ECSTest : public ::testing::Test {
    protected:
        World m_World;

        void SetUp() override {
            m_World.Clear();
        }

        void TearDown() override {
            m_World.Clear();
        }
    };

    struct Position {
        float x = 0.0f;
        float y = 0.0f;
    };

    struct Velocity {
        float dx = 0.0f;
        float dy = 0.0f;
    };

    struct Health {
        int32 value = 100;

        Health() = default;
        explicit Health(int32 v) : value(v) {}
    };

    struct Tag {};
} // namespace

// ---------------------------------------------------------------------
// Entity lifecycle
// ---------------------------------------------------------------------

TEST_F(ECSTest, CreateEntityReturnsAliveEntity) {
    Entity e = m_World.CreateEntity();
    EXPECT_TRUE(m_World.IsAlive(e));
}

TEST_F(ECSTest, CreateEntityDoesNotReturnNullEntity) {
    Entity e = m_World.CreateEntity();
    EXPECT_FALSE(e == NullEntity);
}

TEST_F(ECSTest, NullEntityIsNotAlive) {
    EXPECT_FALSE(m_World.IsAlive(NullEntity));
}

TEST_F(ECSTest, CreateEntityReturnsDistinctEntities) {
    Entity a = m_World.CreateEntity();
    Entity b = m_World.CreateEntity();

    EXPECT_FALSE(a == b);
    EXPECT_TRUE(m_World.IsAlive(a));
    EXPECT_TRUE(m_World.IsAlive(b));
}

TEST_F(ECSTest, DestroyEntityMakesItNotAlive) {
    Entity e = m_World.CreateEntity();
    m_World.DestroyEntity(e);
    EXPECT_FALSE(m_World.IsAlive(e));
}

TEST_F(ECSTest, DestroyingOneEntityDoesNotAffectOthers) {
    Entity a = m_World.CreateEntity();
    Entity b = m_World.CreateEntity();

    m_World.DestroyEntity(a);

    EXPECT_FALSE(m_World.IsAlive(a));
    EXPECT_TRUE(m_World.IsAlive(b));
}

TEST_F(ECSTest, RecycledEntityIndexProducesNewGeneration) {
    Entity a = m_World.CreateEntity();
    m_World.DestroyEntity(a);

    Entity b = m_World.CreateEntity();

    if (a.index == b.index) {
        EXPECT_NE(a.generation, b.generation);
    }

    EXPECT_FALSE(m_World.IsAlive(a));
    EXPECT_TRUE(m_World.IsAlive(b));
}

TEST_F(ECSTest, StaleHandleAfterDestroyDoesNotAliasNewEntityAtSameIndex) {
    Entity a = m_World.CreateEntity();
    m_World.DestroyEntity(a);
    Entity b = m_World.CreateEntity();

    EXPECT_FALSE(m_World.IsAlive(a));
    EXPECT_TRUE(m_World.IsAlive(b));
}

// ---------------------------------------------------------------------
// Add / Get / Has / Remove
// ---------------------------------------------------------------------

TEST_F(ECSTest, AddComponentMakesHasReturnTrue) {
    Entity e = m_World.CreateEntity();
    m_World.Add<Position>(e, 1.0f, 2.0f);

    EXPECT_TRUE(m_World.Has<Position>(e));
}

TEST_F(ECSTest, HasReturnsFalseWhenComponentNotAdded) {
    Entity e = m_World.CreateEntity();
    EXPECT_FALSE(m_World.Has<Position>(e));
}

TEST_F(ECSTest, HasReturnsFalseForUnknownComponentTypeNeverAdded) {
    Entity e = m_World.CreateEntity();
    EXPECT_FALSE(m_World.Has<Position>(e));
}

TEST_F(ECSTest, AddReturnsReferenceToConstructedComponent) {
    Entity e = m_World.CreateEntity();
    Position& pos = m_World.Add<Position>(e, 3.0f, 4.0f);

    EXPECT_FLOAT_EQ(pos.x, 3.0f);
    EXPECT_FLOAT_EQ(pos.y, 4.0f);
}

TEST_F(ECSTest, AddAggregateComponentWithBraceStyleArgs) {
    Entity e = m_World.CreateEntity();
    Velocity& vel = m_World.Add<Velocity>(e, 1.5f, -2.5f);

    EXPECT_FLOAT_EQ(vel.dx, 1.5f);
    EXPECT_FLOAT_EQ(vel.dy, -2.5f);
}

TEST_F(ECSTest, GetReturnsPreviouslyAddedComponent) {
    Entity e = m_World.CreateEntity();
    m_World.Add<Position>(e, 5.0f, 6.0f);

    const Position& pos = m_World.Get<Position>(e);
    EXPECT_FLOAT_EQ(pos.x, 5.0f);
    EXPECT_FLOAT_EQ(pos.y, 6.0f);
}

TEST_F(ECSTest, GetAllowsMutationOfComponent) {
    Entity e = m_World.CreateEntity();
    m_World.Add<Position>(e, 0.0f, 0.0f);

    m_World.Get<Position>(e).x = 42.0f;

    EXPECT_FLOAT_EQ(m_World.Get<Position>(e).x, 42.0f);
}

TEST_F(ECSTest, GetOnConstWorldReturnsComponent) {
    Entity e = m_World.CreateEntity();
    m_World.Add<Position>(e, 7.0f, 8.0f);

    const World& constWorld = m_World;
    const Position& pos = constWorld.Get<Position>(e);

    EXPECT_FLOAT_EQ(pos.x, 7.0f);
    EXPECT_FLOAT_EQ(pos.y, 8.0f);
}

TEST_F(ECSTest, TryGetReturnsNullptrWhenComponentMissing) {
    Entity e = m_World.CreateEntity();
    EXPECT_EQ(m_World.TryGet<Position>(e), nullptr);
}

TEST_F(ECSTest, TryGetReturnsNullptrWhenPoolNeverCreated) {
    Entity e = m_World.CreateEntity();
    EXPECT_EQ(m_World.TryGet<Health>(e), nullptr);
}

TEST_F(ECSTest, TryGetReturnsValidPointerWhenComponentPresent) {
    Entity e = m_World.CreateEntity();
    m_World.Add<Position>(e, 1.0f, 1.0f);

    Position* pos = m_World.TryGet<Position>(e);
    ASSERT_NE(pos, nullptr);
    EXPECT_FLOAT_EQ(pos->x, 1.0f);
}

TEST_F(ECSTest, TryGetOnConstWorldReturnsConstPointer) {
    Entity e = m_World.CreateEntity();
    m_World.Add<Position>(e, 9.0f, 9.0f);

    const World& constWorld = m_World;
    const Position* pos = constWorld.TryGet<Position>(e);

    ASSERT_NE(pos, nullptr);
    EXPECT_FLOAT_EQ(pos->x, 9.0f);
}

TEST_F(ECSTest, RemoveComponentMakesHasReturnFalse) {
    Entity e = m_World.CreateEntity();
    m_World.Add<Position>(e, 1.0f, 1.0f);
    m_World.Remove<Position>(e);

    EXPECT_FALSE(m_World.Has<Position>(e));
    EXPECT_EQ(m_World.TryGet<Position>(e), nullptr);
}

TEST_F(ECSTest, RemovingOneComponentDoesNotAffectOtherComponents) {
    Entity e = m_World.CreateEntity();
    m_World.Add<Position>(e, 1.0f, 1.0f);
    m_World.Add<Velocity>(e, 2.0f, 2.0f);

    m_World.Remove<Position>(e);

    EXPECT_FALSE(m_World.Has<Position>(e));
    EXPECT_TRUE(m_World.Has<Velocity>(e));
}

TEST_F(ECSTest, MultipleComponentTypesOnSameEntityAreIndependent) {
    Entity e = m_World.CreateEntity();
    m_World.Add<Position>(e, 1.0f, 2.0f);
    m_World.Add<Velocity>(e, 3.0f, 4.0f);
    m_World.Add<Health>(e, 50);

    EXPECT_TRUE(m_World.Has<Position>(e));
    EXPECT_TRUE(m_World.Has<Velocity>(e));
    EXPECT_TRUE(m_World.Has<Health>(e));
    EXPECT_EQ(m_World.Get<Health>(e).value, 50);
}

TEST_F(ECSTest, TagLikeEmptyComponentCanBeAddedAndDetected) {
    Entity e = m_World.CreateEntity();
    m_World.Add<Tag>(e);

    EXPECT_TRUE(m_World.Has<Tag>(e));
}

TEST_F(ECSTest, ComponentsOfSameTypeAreIndependentAcrossEntities) {
    Entity a = m_World.CreateEntity();
    Entity b = m_World.CreateEntity();

    m_World.Add<Health>(a, 10);
    m_World.Add<Health>(b, 20);

    EXPECT_EQ(m_World.Get<Health>(a).value, 10);
    EXPECT_EQ(m_World.Get<Health>(b).value, 20);
}

// ---------------------------------------------------------------------
// Reserve
// ---------------------------------------------------------------------

TEST_F(ECSTest, ReserveDoesNotCreateComponentsOrEntities) {
    m_World.Reserve<Position, Velocity>(128);

    Entity e = m_World.CreateEntity();
    EXPECT_FALSE(m_World.Has<Position>(e));
    EXPECT_FALSE(m_World.Has<Velocity>(e));
}

TEST_F(ECSTest, ReserveThenAddStillWorksCorrectly) {
    m_World.Reserve<Position>(64);

    Entity e = m_World.CreateEntity();
    m_World.Add<Position>(e, 1.0f, 2.0f);

    EXPECT_TRUE(m_World.Has<Position>(e));
    EXPECT_FLOAT_EQ(m_World.Get<Position>(e).x, 1.0f);
}

TEST_F(ECSTest, ReserveWithSingleComponentTypeCompiles) {
    m_World.Reserve<Health>(16);
    SUCCEED();
}

// ---------------------------------------------------------------------
// Query (all-of semantics)
// ---------------------------------------------------------------------

TEST_F(ECSTest, QuerySingleComponentReturnsOnlyMatchingEntities) {
    Entity a = m_World.CreateEntity();
    Entity b = m_World.CreateEntity();
    Entity c = m_World.CreateEntity();

    m_World.Add<Position>(a, 0.0f, 0.0f);
    m_World.Add<Position>(c, 0.0f, 0.0f);
    m_World.Add<Velocity>(b, 0.0f, 0.0f);

    std::vector<Entity> results = m_World.Query<Position>();

    EXPECT_EQ(results.size(), 2u);
    EXPECT_TRUE(Contains(results, a));
    EXPECT_TRUE(Contains(results, c));
    EXPECT_FALSE(Contains(results, b));
}

TEST_F(ECSTest, QueryMultipleComponentsRequiresAllOfThem) {
    Entity a = m_World.CreateEntity();
    Entity b = m_World.CreateEntity();

    m_World.Add<Position>(a, 0.0f, 0.0f);
    m_World.Add<Velocity>(a, 0.0f, 0.0f);

    m_World.Add<Position>(b, 0.0f, 0.0f);

    std::vector<Entity> results = m_World.Query<Position, Velocity>();

    ASSERT_EQ(results.size(), 1u);
    EXPECT_TRUE(results.front() == a);
}

TEST_F(ECSTest, QueryReturnsEmptyWhenNoEntitiesMatch) {
    Entity a = m_World.CreateEntity();
    m_World.Add<Position>(a, 0.0f, 0.0f);

    std::vector<Entity> results = m_World.Query<Velocity>();
    EXPECT_TRUE(results.empty());
}

TEST_F(ECSTest, QueryReturnsEmptyWhenPoolNeverCreated) {
    m_World.CreateEntity();
    std::vector<Entity> results = m_World.Query<Health>();
    EXPECT_TRUE(results.empty());
}

TEST_F(ECSTest, QueryExcludesDestroyedEntities) {
    Entity a = m_World.CreateEntity();
    Entity b = m_World.CreateEntity();

    m_World.Add<Position>(a, 0.0f, 0.0f);
    m_World.Add<Position>(b, 0.0f, 0.0f);

    m_World.DestroyEntity(a);

    std::vector<Entity> results = m_World.Query<Position>();

    EXPECT_FALSE(Contains(results, a));
    EXPECT_TRUE(Contains(results, b));
}

// ---------------------------------------------------------------------
// QueryAny (any-of semantics)
// ---------------------------------------------------------------------

TEST_F(ECSTest, QueryAnyReturnsEntitiesWithAtLeastOneComponent) {
    Entity a = m_World.CreateEntity();
    Entity b = m_World.CreateEntity();
    Entity c = m_World.CreateEntity();

    m_World.Add<Position>(a, 0.0f, 0.0f);
    m_World.Add<Velocity>(b, 0.0f, 0.0f);

    std::vector<Entity> results = m_World.QueryAny<Position, Velocity>();

    EXPECT_EQ(results.size(), 2u);
    EXPECT_TRUE(Contains(results, a));
    EXPECT_TRUE(Contains(results, b));
    EXPECT_FALSE(Contains(results, c));
}

TEST_F(ECSTest, QueryAnyDoesNotDuplicateEntityWithBothComponents) {
    Entity a = m_World.CreateEntity();
    m_World.Add<Position>(a, 0.0f, 0.0f);
    m_World.Add<Velocity>(a, 0.0f, 0.0f);

    std::vector<Entity> results = m_World.QueryAny<Position, Velocity>();

    EXPECT_EQ(std::ranges::count_if(results, [&](Entity e) { return e == a; }), 1);
}

TEST_F(ECSTest, QueryAnyReturnsEmptyWhenNoEntitiesHaveAnyComponent) {
    m_World.CreateEntity();
    std::vector<Entity> results = m_World.QueryAny<Position, Velocity>();
    EXPECT_TRUE(results.empty());
}

// ---------------------------------------------------------------------
// View
// ---------------------------------------------------------------------

TEST_F(ECSTest, ViewIteratesOnlyMatchingEntities) {
    Entity a = m_World.CreateEntity();
    Entity b = m_World.CreateEntity();

    m_World.Add<Position>(a, 1.0f, 1.0f);
    m_World.Add<Velocity>(a, 2.0f, 2.0f);

    m_World.Add<Position>(b, 3.0f, 3.0f);

    usize count = 0;
    for (auto [e, pos, vel] : m_World.View<Position, Velocity>()) {
        EXPECT_TRUE(e == a);
        EXPECT_FALSE(e == b);
        ++count;
    }

    EXPECT_EQ(count, 1u);
}

TEST_F(ECSTest, ViewSingleComponentVisitsAllMatchingEntities) {
    Entity a = m_World.CreateEntity();
    Entity b = m_World.CreateEntity();

    m_World.Add<Health>(a, 1);
    m_World.Add<Health>(b, 2);

    usize count = 0;
    for (auto [e, health] : m_World.View<Health>()) {
        EXPECT_TRUE(e == a || e == b);
        ++count;
    }

    EXPECT_EQ(count, 2u);
}

TEST_F(ECSTest, ViewOverEmptyPoolYieldsNoIterations) {
    m_World.CreateEntity();

    usize count = 0;
    for ([[maybe_unused]] auto [e, pos] : m_World.View<Position>()) {
        ++count;
    }

    EXPECT_EQ(count, 0u);
}
