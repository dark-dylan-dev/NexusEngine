// SPDX-License-Identifier: MIT

#include <gtest/gtest.h>

import NE.Engine.Core.Types;
import NE.Engine.Math.Quaternion;
import NE.Engine.Math.Vec;

import std;

using namespace Nexus;

namespace {
    constexpr float32 kPi = std::numbers::pi_v<float32>;

    template <std::floating_point T>
    void ExpectNear(const Quaternion<T>& a, const Quaternion<T>& b, T tolerance = static_cast<T>(1e-4)) {
        EXPECT_NEAR(a.X(), b.X(), tolerance);
        EXPECT_NEAR(a.Y(), b.Y(), tolerance);
        EXPECT_NEAR(a.Z(), b.Z(), tolerance);
        EXPECT_NEAR(a.W(), b.W(), tolerance);
    }

    template <std::floating_point T>
    void ExpectNear(const Vec3<T>& a, const Vec3<T>& b, T tolerance = static_cast<T>(1e-4)) {
        EXPECT_NEAR(a[0], b[0], tolerance);
        EXPECT_NEAR(a[1], b[1], tolerance);
        EXPECT_NEAR(a[2], b[2], tolerance);
    }
} // namespace

// =========================================================================
// Construction
// =========================================================================

TEST(QuaternionTest, DefaultConstruction) {
    constexpr Quaternion<float32> q{};

    EXPECT_FLOAT_EQ(q.X(), 0.0f);
    EXPECT_FLOAT_EQ(q.Y(), 0.0f);
    EXPECT_FLOAT_EQ(q.Z(), 0.0f);
    EXPECT_FLOAT_EQ(q.W(), 1.0f);
}

TEST(QuaternionTest, ValueConstruction) {
    constexpr Quaternion<float32> q{1.0f, 2.0f, 3.0f, 4.0f};

    EXPECT_FLOAT_EQ(q.X(), 1.0f);
    EXPECT_FLOAT_EQ(q.Y(), 2.0f);
    EXPECT_FLOAT_EQ(q.Z(), 3.0f);
    EXPECT_FLOAT_EQ(q.W(), 4.0f);
}

TEST(QuaternionTest, Identity) {
    constexpr Quaternion<float32> q = Quaternion<float32>::Identity();

    EXPECT_FLOAT_EQ(q.X(), 0.0f);
    EXPECT_FLOAT_EQ(q.Y(), 0.0f);
    EXPECT_FLOAT_EQ(q.Z(), 0.0f);
    EXPECT_FLOAT_EQ(q.W(), 1.0f);
}

TEST(QuaternionTest, FromAxisAngle) {
    constexpr Vec3<float32> axis{0.0f, 1.0f, 0.0f};
    const Quaternion<float32> q = Quaternion<float32>::FromAxisAngle(axis, kPi / 2.0f);

    EXPECT_NEAR(q.X(), 0.0f, 1e-4f);
    EXPECT_NEAR(q.Y(), std::sin(kPi / 4.0f), 1e-4f);
    EXPECT_NEAR(q.Z(), 0.0f, 1e-4f);
    EXPECT_NEAR(q.W(), std::cos(kPi / 4.0f), 1e-4f);
}

TEST(QuaternionTest, FromAxisAngleNormalizesAxis) {
    constexpr Vec3<float32> axis{0.0f, 5.0f, 0.0f};
    const Quaternion<float32> q = Quaternion<float32>::FromAxisAngle(axis, kPi / 2.0f);

    EXPECT_NEAR(q.Length(), 1.0f, 1e-4f);
    EXPECT_NEAR(q.Y(), std::sin(kPi / 4.0f), 1e-4f);
}

TEST(QuaternionTest, FromAxisAngleZeroAxis) {
    constexpr Vec3<float32> axis{0.0f, 0.0f, 0.0f};
    const Quaternion<float32> q = Quaternion<float32>::FromAxisAngle(axis, kPi / 2.0f);

    EXPECT_NEAR(q.X(), 0.0f, 1e-4f);
    EXPECT_NEAR(q.Y(), 0.0f, 1e-4f);
    EXPECT_NEAR(q.Z(), 0.0f, 1e-4f);
}

TEST(QuaternionTest, AxisAngleConstructor) {
    constexpr Vec3<float32> axis{0.0f, 0.0f, 1.0f};
    const Quaternion<float32> q{axis, kPi};

    ExpectNear(q, Quaternion<float32>::FromAxisAngle(axis, kPi));
}

TEST(QuaternionTest, FromEulerIdentity) {
    const Quaternion<float32> q = Quaternion<float32>::FromEuler(0.0f, 0.0f, 0.0f);

    ExpectNear(q, Quaternion<float32>::Identity());
}

TEST(QuaternionTest, FromEulerMatchesAxisAngleForSingleAxis) {
    constexpr float32 angle = kPi / 3.0f;

    const Quaternion<float32> fromEuler = Quaternion<float32>::FromEuler(angle, 0.0f, 0.0f);
    const Quaternion<float32> fromAxisAngle =
        Quaternion<float32>::FromAxisAngle(Vec3<float32>{1.0f, 0.0f, 0.0f}, angle);

    ExpectNear(fromEuler, fromAxisAngle);
}

TEST(QuaternionTest, MixedValueTypes) {
    constexpr Quaternion<float64> q{1, 2.5, 3, 4};

    EXPECT_DOUBLE_EQ(q.X(), 1.0);
    EXPECT_DOUBLE_EQ(q.Y(), 2.5);
    EXPECT_DOUBLE_EQ(q.Z(), 3.0);
    EXPECT_DOUBLE_EQ(q.W(), 4.0);
}

// =========================================================================
// Access
// =========================================================================

TEST(QuaternionTest, ComponentAccess) {
    Quaternion<float32> q{1.0f, 2.0f, 3.0f, 4.0f};

    q.X() = 10.0f;
    q.Y() = 20.0f;
    q.Z() = 30.0f;
    q.W() = 40.0f;

    EXPECT_FLOAT_EQ(q.X(), 10.0f);
    EXPECT_FLOAT_EQ(q.Y(), 20.0f);
    EXPECT_FLOAT_EQ(q.Z(), 30.0f);
    EXPECT_FLOAT_EQ(q.W(), 40.0f);
}

TEST(QuaternionTest, ConstComponentAccess) {
    constexpr Quaternion<float32> q{1.0f, 2.0f, 3.0f, 4.0f};

    EXPECT_FLOAT_EQ(q.X(), 1.0f);
    EXPECT_FLOAT_EQ(q.Y(), 2.0f);
    EXPECT_FLOAT_EQ(q.Z(), 3.0f);
    EXPECT_FLOAT_EQ(q.W(), 4.0f);
}

TEST(QuaternionTest, Data) {
    Quaternion<float32> q{1.0f, 2.0f, 3.0f, 4.0f};

    float32* data = q.Data();

    EXPECT_NE(data, nullptr);

    data[0] = 10.0f;
    data[1] = 20.0f;
    data[2] = 30.0f;
    data[3] = 40.0f;

    EXPECT_FLOAT_EQ(q.X(), 10.0f);
    EXPECT_FLOAT_EQ(q.Y(), 20.0f);
    EXPECT_FLOAT_EQ(q.Z(), 30.0f);
    EXPECT_FLOAT_EQ(q.W(), 40.0f);
}

TEST(QuaternionTest, Size) {
    EXPECT_EQ((Quaternion<float32>::Size()), 4);
    EXPECT_EQ((Quaternion<float64>::Size()), 4);
}

// =========================================================================
// Equality
// =========================================================================

TEST(QuaternionTest, Equality) {
    constexpr Quaternion<float32> a{1.0f, 2.0f, 3.0f, 4.0f};
    constexpr Quaternion<float32> b{1.0f, 2.0f, 3.0f, 4.0f};

    EXPECT_TRUE(a == b);
}

TEST(QuaternionTest, Inequality) {
    constexpr Quaternion<float32> a{1.0f, 2.0f, 3.0f, 4.0f};
    constexpr Quaternion<float32> b{1.0f, 2.0f, 3.0f, 5.0f};

    EXPECT_FALSE(a == b);
}

TEST(QuaternionTest, EqualityDifferentTypes) {
    constexpr Quaternion<float32> a{1.0f, 2.0f, 3.0f, 4.0f};
    constexpr Quaternion<float64> b{1.0, 2.0, 3.0, 4.0};

    EXPECT_TRUE(a == b);
}

// =========================================================================
// Addition
// =========================================================================

TEST(QuaternionTest, Addition) {
    constexpr Quaternion<float32> a{1.0f, 2.0f, 3.0f, 4.0f};
    constexpr Quaternion<float32> b{4.0f, 5.0f, 6.0f, 7.0f};

    constexpr auto result = a + b;

    EXPECT_FLOAT_EQ(result.X(), 5.0f);
    EXPECT_FLOAT_EQ(result.Y(), 7.0f);
    EXPECT_FLOAT_EQ(result.Z(), 9.0f);
    EXPECT_FLOAT_EQ(result.W(), 11.0f);
}

TEST(QuaternionTest, AdditionCommonType) {
    constexpr Quaternion<float32> a{1.0f, 2.0f, 3.0f, 4.0f};
    constexpr Quaternion<float64> b{0.5, 1.5, 2.5, 3.5};

    constexpr auto result = a + b;

    static_assert(std::is_same_v<decltype(result), const Quaternion<float64>>);

    EXPECT_DOUBLE_EQ(result.X(), 1.5);
    EXPECT_DOUBLE_EQ(result.Y(), 3.5);
    EXPECT_DOUBLE_EQ(result.Z(), 5.5);
    EXPECT_DOUBLE_EQ(result.W(), 7.5);
}

TEST(QuaternionTest, AdditionAssignment) {
    Quaternion<float32> a{1.0f, 2.0f, 3.0f, 4.0f};
    constexpr Quaternion<float32> b{4.0f, 5.0f, 6.0f, 7.0f};

    a += b;

    EXPECT_FLOAT_EQ(a.X(), 5.0f);
    EXPECT_FLOAT_EQ(a.Y(), 7.0f);
    EXPECT_FLOAT_EQ(a.Z(), 9.0f);
    EXPECT_FLOAT_EQ(a.W(), 11.0f);
}

TEST(QuaternionTest, MixedTypeAdditionAssignment) {
    Quaternion<float64> a{1.0, 2.0, 3.0, 4.0};
    constexpr Quaternion<float32> b{4.0f, 5.0f, 6.0f, 7.0f};

    a += b;

    EXPECT_DOUBLE_EQ(a.X(), 5.0);
    EXPECT_DOUBLE_EQ(a.Y(), 7.0);
    EXPECT_DOUBLE_EQ(a.Z(), 9.0);
    EXPECT_DOUBLE_EQ(a.W(), 11.0);
}

// =========================================================================
// Subtraction
// =========================================================================

TEST(QuaternionTest, Subtraction) {
    constexpr Quaternion<float32> a{5.0f, 7.0f, 9.0f, 11.0f};
    constexpr Quaternion<float32> b{1.0f, 2.0f, 3.0f, 4.0f};

    constexpr auto result = a - b;

    EXPECT_FLOAT_EQ(result.X(), 4.0f);
    EXPECT_FLOAT_EQ(result.Y(), 5.0f);
    EXPECT_FLOAT_EQ(result.Z(), 6.0f);
    EXPECT_FLOAT_EQ(result.W(), 7.0f);
}

TEST(QuaternionTest, SubtractionCommonType) {
    constexpr Quaternion<float32> a{5.0f, 7.0f, 9.0f, 11.0f};
    constexpr Quaternion<float64> b{0.5, 1.5, 2.5, 3.5};

    constexpr auto result = a - b;

    static_assert(std::is_same_v<decltype(result), const Quaternion<float64>>);

    EXPECT_DOUBLE_EQ(result.X(), 4.5);
    EXPECT_DOUBLE_EQ(result.Y(), 5.5);
    EXPECT_DOUBLE_EQ(result.Z(), 6.5);
    EXPECT_DOUBLE_EQ(result.W(), 7.5);
}

TEST(QuaternionTest, SubtractionAssignment) {
    Quaternion<float32> a{5.0f, 7.0f, 9.0f, 11.0f};
    constexpr Quaternion<float32> b{1.0f, 2.0f, 3.0f, 4.0f};

    a -= b;

    EXPECT_FLOAT_EQ(a.X(), 4.0f);
    EXPECT_FLOAT_EQ(a.Y(), 5.0f);
    EXPECT_FLOAT_EQ(a.Z(), 6.0f);
    EXPECT_FLOAT_EQ(a.W(), 7.0f);
}

// =========================================================================
// Hamilton product
// =========================================================================

TEST(QuaternionTest, MultiplicationByIdentity) {
    constexpr Quaternion<float32> q{1.0f, 2.0f, 3.0f, 4.0f};
    constexpr Quaternion<float32> identity = Quaternion<float32>::Identity();

    constexpr auto result = q * identity;

    EXPECT_FLOAT_EQ(result.X(), q.X());
    EXPECT_FLOAT_EQ(result.Y(), q.Y());
    EXPECT_FLOAT_EQ(result.Z(), q.Z());
    EXPECT_FLOAT_EQ(result.W(), q.W());
}

TEST(QuaternionTest, MultiplicationIsNonCommutative) {
    const Quaternion<float32> a = Quaternion<float32>::FromAxisAngle(Vec3<float32>{1.0f, 0.0f, 0.0f}, kPi / 2.0f);
    const Quaternion<float32> b = Quaternion<float32>::FromAxisAngle(Vec3<float32>{0.0f, 1.0f, 0.0f}, kPi / 2.0f);

    const Quaternion<float32> ab = a * b;
    const Quaternion<float32> ba = b * a;

    EXPECT_FALSE((ab.X() == ba.X()) && (ab.Y() == ba.Y()) && (ab.Z() == ba.Z()) && (ab.W() == ba.W()));
}

TEST(QuaternionTest, MultiplicationCombinesRotations) {
    // Two 90 degree rotations around Z should equal one 180 degree rotation around Z.
    const Quaternion<float32> quarterTurn =
        Quaternion<float32>::FromAxisAngle(Vec3<float32>{0.0f, 0.0f, 1.0f}, kPi / 2.0f);
    const Quaternion<float32> halfTurn = Quaternion<float32>::FromAxisAngle(Vec3<float32>{0.0f, 0.0f, 1.0f}, kPi);

    const Quaternion<float32> combined = quarterTurn * quarterTurn;

    ExpectNear(combined, halfTurn);
}

TEST(QuaternionTest, MultiplicationCommonType) {
    constexpr Quaternion<float32> a{0.0f, 0.0f, 0.0f, 1.0f};
    constexpr Quaternion<float64> b{1.0, 0.0, 0.0, 0.0};

    constexpr auto result = a * b;

    static_assert(std::is_same_v<decltype(result), const Quaternion<float64>>);

    EXPECT_DOUBLE_EQ(result.X(), 1.0);
    EXPECT_DOUBLE_EQ(result.Y(), 0.0);
    EXPECT_DOUBLE_EQ(result.Z(), 0.0);
    EXPECT_DOUBLE_EQ(result.W(), 0.0);
}

TEST(QuaternionTest, MultiplicationAssignment) {
    Quaternion<float32> a = Quaternion<float32>::FromAxisAngle(Vec3<float32>{0.0f, 0.0f, 1.0f}, kPi / 2.0f);
    const Quaternion<float32> b = Quaternion<float32>::FromAxisAngle(Vec3<float32>{0.0f, 0.0f, 1.0f}, kPi / 2.0f);

    a *= b;

    ExpectNear(a, Quaternion<float32>::FromAxisAngle(Vec3<float32>{0.0f, 0.0f, 1.0f}, kPi));
}

// =========================================================================
// Scalar multiplication
// =========================================================================

TEST(QuaternionTest, ScalarMultiplication) {
    constexpr Quaternion<float32> q{1.0f, 2.0f, 3.0f, 4.0f};

    constexpr auto result = q * 2.0f;

    EXPECT_FLOAT_EQ(result.X(), 2.0f);
    EXPECT_FLOAT_EQ(result.Y(), 4.0f);
    EXPECT_FLOAT_EQ(result.Z(), 6.0f);
    EXPECT_FLOAT_EQ(result.W(), 8.0f);
}

TEST(QuaternionTest, ScalarMultiplicationCommonType) {
    constexpr Quaternion<float32> q{1.0f, 2.0f, 3.0f, 4.0f};

    constexpr auto result = q * 0.5;

    static_assert(std::is_same_v<decltype(result), const Quaternion<float64>>);

    EXPECT_DOUBLE_EQ(result.X(), 0.5);
    EXPECT_DOUBLE_EQ(result.Y(), 1.0);
    EXPECT_DOUBLE_EQ(result.Z(), 1.5);
    EXPECT_DOUBLE_EQ(result.W(), 2.0);
}

TEST(QuaternionTest, ScalarMultiplicationReversed) {
    constexpr Quaternion<float32> q{1.0f, 2.0f, 3.0f, 4.0f};

    constexpr auto result = 2.0f * q;

    EXPECT_FLOAT_EQ(result.X(), 2.0f);
    EXPECT_FLOAT_EQ(result.Y(), 4.0f);
    EXPECT_FLOAT_EQ(result.Z(), 6.0f);
    EXPECT_FLOAT_EQ(result.W(), 8.0f);
}

TEST(QuaternionTest, ScalarMultiplicationAssignment) {
    Quaternion<float32> q{1.0f, 2.0f, 3.0f, 4.0f};

    q *= 3.0f;

    EXPECT_FLOAT_EQ(q.X(), 3.0f);
    EXPECT_FLOAT_EQ(q.Y(), 6.0f);
    EXPECT_FLOAT_EQ(q.Z(), 9.0f);
    EXPECT_FLOAT_EQ(q.W(), 12.0f);
}

// =========================================================================
// Scalar division
// =========================================================================

TEST(QuaternionTest, ScalarDivision) {
    constexpr Quaternion<float32> q{2.0f, 4.0f, 6.0f, 8.0f};

    constexpr auto result = q / 2.0f;

    EXPECT_FLOAT_EQ(result.X(), 1.0f);
    EXPECT_FLOAT_EQ(result.Y(), 2.0f);
    EXPECT_FLOAT_EQ(result.Z(), 3.0f);
    EXPECT_FLOAT_EQ(result.W(), 4.0f);
}

TEST(QuaternionTest, ScalarDivisionCommonType) {
    constexpr Quaternion<float32> q{1.0f, 2.0f, 3.0f, 4.0f};

    constexpr auto result = q / 2.0;

    static_assert(std::is_same_v<decltype(result), const Quaternion<float64>>);

    EXPECT_DOUBLE_EQ(result.X(), 0.5);
    EXPECT_DOUBLE_EQ(result.Y(), 1.0);
    EXPECT_DOUBLE_EQ(result.Z(), 1.5);
    EXPECT_DOUBLE_EQ(result.W(), 2.0);
}

TEST(QuaternionTest, ScalarDivisionAssignment) {
    Quaternion<float32> q{2.0f, 4.0f, 6.0f, 8.0f};

    q /= 2.0f;

    EXPECT_FLOAT_EQ(q.X(), 1.0f);
    EXPECT_FLOAT_EQ(q.Y(), 2.0f);
    EXPECT_FLOAT_EQ(q.Z(), 3.0f);
    EXPECT_FLOAT_EQ(q.W(), 4.0f);
}

// =========================================================================
// Unary operators
// =========================================================================

TEST(QuaternionTest, UnaryPlus) {
    constexpr Quaternion<float32> q{1.0f, 2.0f, 3.0f, 4.0f};

    constexpr auto result = +q;

    EXPECT_FLOAT_EQ(result.X(), 1.0f);
    EXPECT_FLOAT_EQ(result.Y(), 2.0f);
    EXPECT_FLOAT_EQ(result.Z(), 3.0f);
    EXPECT_FLOAT_EQ(result.W(), 4.0f);
}

TEST(QuaternionTest, UnaryMinus) {
    constexpr Quaternion<float32> q{1.0f, -2.0f, 3.0f, -4.0f};

    constexpr auto result = -q;

    EXPECT_FLOAT_EQ(result.X(), -1.0f);
    EXPECT_FLOAT_EQ(result.Y(), 2.0f);
    EXPECT_FLOAT_EQ(result.Z(), -3.0f);
    EXPECT_FLOAT_EQ(result.W(), 4.0f);
}

// =========================================================================
// Length / normalization
// =========================================================================

TEST(QuaternionTest, LengthSquared) {
    constexpr Quaternion<float32> q{1.0f, 2.0f, 2.0f, 4.0f};

    EXPECT_FLOAT_EQ(q.LengthSquared(), 25.0f);
}

TEST(QuaternionTest, Length) {
    constexpr Quaternion<float32> q{0.0f, 0.0f, 0.0f, 3.0f};

    EXPECT_FLOAT_EQ(q.Length(), 3.0f);
}

TEST(QuaternionTest, IdentityLengthIsOne) {
    constexpr Quaternion<float32> q = Quaternion<float32>::Identity();

    EXPECT_FLOAT_EQ(q.Length(), 1.0f);
}

TEST(QuaternionTest, Normalized) {
    constexpr Quaternion<float32> q{0.0f, 0.0f, 0.0f, 2.0f};

    const Quaternion<float32> result = q.Normalized();

    EXPECT_FLOAT_EQ(result.Length(), 1.0f);
    EXPECT_FLOAT_EQ(result.W(), 1.0f);
}

TEST(QuaternionTest, NormalizedDoesNotMutate) {
    constexpr Quaternion<float32> q{1.0f, 2.0f, 3.0f, 4.0f};

    const Quaternion<float32> result = q.Normalized();

    EXPECT_FLOAT_EQ(q.X(), 1.0f);
    EXPECT_FLOAT_EQ(q.Y(), 2.0f);
    EXPECT_FLOAT_EQ(q.Z(), 3.0f);
    EXPECT_FLOAT_EQ(q.W(), 4.0f);
    EXPECT_NEAR(result.Length(), 1.0f, 1e-4f);
}

TEST(QuaternionTest, NormalizedZeroQuaternion) {
    constexpr Quaternion<float32> q{0.0f, 0.0f, 0.0f, 0.0f};

    const Quaternion<float32> result = q.Normalized();

    EXPECT_FLOAT_EQ(result.X(), 0.0f);
    EXPECT_FLOAT_EQ(result.Y(), 0.0f);
    EXPECT_FLOAT_EQ(result.Z(), 0.0f);
    EXPECT_FLOAT_EQ(result.W(), 0.0f);
}

TEST(QuaternionTest, Normalize) {
    Quaternion<float32> q{1.0f, 2.0f, 3.0f, 4.0f};

    q.Normalize();

    EXPECT_NEAR(q.Length(), 1.0f, 1e-4f);
}

// =========================================================================
// Conjugate / inverse
// =========================================================================

TEST(QuaternionTest, Conjugate) {
    constexpr Quaternion<float32> q{1.0f, 2.0f, 3.0f, 4.0f};

    constexpr auto result = q.Conjugate();

    EXPECT_FLOAT_EQ(result.X(), -1.0f);
    EXPECT_FLOAT_EQ(result.Y(), -2.0f);
    EXPECT_FLOAT_EQ(result.Z(), -3.0f);
    EXPECT_FLOAT_EQ(result.W(), 4.0f);
}

TEST(QuaternionTest, InverseOfUnitQuaternionEqualsConjugate) {
    const Quaternion<float32> q = Quaternion<float32>::FromAxisAngle(Vec3<float32>{0.0f, 1.0f, 0.0f}, kPi / 3.0f);

    ExpectNear(q.Inverse(), q.Conjugate());
}

TEST(QuaternionTest, InverseTimesSelfIsIdentity) {
    const Quaternion<float32> q = Quaternion<float32>::FromAxisAngle(Vec3<float32>{1.0f, 1.0f, 0.0f}, kPi / 4.0f);

    const Quaternion<float32> result = q * q.Inverse();

    ExpectNear(result, Quaternion<float32>::Identity());
}

TEST(QuaternionTest, InverseOfZeroQuaternionReturnsConjugate) {
    constexpr Quaternion<float32> q{0.0f, 0.0f, 0.0f, 0.0f};

    constexpr auto result = q.Inverse();

    EXPECT_FLOAT_EQ(result.X(), 0.0f);
    EXPECT_FLOAT_EQ(result.Y(), 0.0f);
    EXPECT_FLOAT_EQ(result.Z(), 0.0f);
    EXPECT_FLOAT_EQ(result.W(), 0.0f);
}

TEST(QuaternionTest, InverseOfNonUnitQuaternion) {
    constexpr Quaternion<float32> q{0.0f, 0.0f, 0.0f, 2.0f};

    constexpr Quaternion<float32> result = q.Inverse();

    EXPECT_NEAR(result.W(), 0.5f, 1e-4f);
}

// =========================================================================
// Vector rotation
// =========================================================================

TEST(QuaternionTest, RotateVectorByIdentity) {
    constexpr Quaternion<float32> identity = Quaternion<float32>::Identity();
    constexpr Vec3<float32> v{1.0f, 2.0f, 3.0f};

    constexpr Vec3<float32> result = identity.RotateVector(v);

    ExpectNear(result, v);
}

TEST(QuaternionTest, RotateVectorNinetyDegreesAroundZ) {
    const Quaternion<float32> q = Quaternion<float32>::FromAxisAngle(Vec3<float32>{0.0f, 0.0f, 1.0f}, kPi / 2.0f);
    constexpr Vec3<float32> v{1.0f, 0.0f, 0.0f};

    const Vec3<float32> result = q.RotateVector(v);

    ExpectNear(result, Vec3<float32>{0.0f, 1.0f, 0.0f});
}

TEST(QuaternionTest, RotateVectorOneEightyAroundX) {
    const Quaternion<float32> q = Quaternion<float32>::FromAxisAngle(Vec3<float32>{1.0f, 0.0f, 0.0f}, kPi);
    constexpr Vec3<float32> v{0.0f, 1.0f, 0.0f};

    const Vec3<float32> result = q.RotateVector(v);

    ExpectNear(result, Vec3<float32>{0.0f, -1.0f, 0.0f});
}

TEST(QuaternionTest, RotateVectorPreservesLength) {
    const Quaternion<float32> q = Quaternion<float32>::FromAxisAngle(Vec3<float32>{1.0f, 1.0f, 1.0f}, kPi / 5.0f);
    constexpr Vec3<float32> v{3.0f, 4.0f, 0.0f};

    const Vec3<float32> result = q.RotateVector(v);

    const float32 originalLength = std::sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    const float32 rotatedLength = std::sqrt(result[0] * result[0] + result[1] * result[1] + result[2] * result[2]);

    EXPECT_NEAR(originalLength, rotatedLength, 1e-4f);
}

TEST(QuaternionTest, OperatorMultiplyRotatesVector) {
    const Quaternion<float32> q = Quaternion<float32>::FromAxisAngle(Vec3<float32>{0.0f, 0.0f, 1.0f}, kPi / 2.0f);
    constexpr Vec3<float32> v{1.0f, 0.0f, 0.0f};

    const Vec3<float32> result = q * v;

    ExpectNear(result, q.RotateVector(v));
}

TEST(QuaternionTest, OperatorMultiplyRotateVectorCommonType) {
    const Quaternion<float32> q = Quaternion<float32>::FromAxisAngle(Vec3<float32>{0.0f, 0.0f, 1.0f}, kPi / 2.0f);
    constexpr Vec3<float64> v{1.0, 0.0, 0.0};

    const auto result = q * v;

    static_assert(std::is_same_v<decltype(result), const Vec3<float64>>);

    EXPECT_NEAR(result[0], 0.0, 1e-4);
    EXPECT_NEAR(result[1], 1.0, 1e-4);
}

// =========================================================================
// Euler conversions
// =========================================================================

TEST(QuaternionTest, EulerRoundTripPitch) {
    const Quaternion<float32> q = Quaternion<float32>::FromEuler(kPi / 6.0f, 0.0f, 0.0f);

    EXPECT_NEAR(q.Pitch(), kPi / 6.0f, 1e-4f);
    EXPECT_NEAR(q.Yaw(), 0.0f, 1e-4f);
    EXPECT_NEAR(q.Roll(), 0.0f, 1e-4f);
}

TEST(QuaternionTest, EulerRoundTripYaw) {
    const Quaternion<float32> q = Quaternion<float32>::FromEuler(0.0f, kPi / 6.0f, 0.0f);

    EXPECT_NEAR(q.Pitch(), 0.0f, 1e-4f);
    EXPECT_NEAR(q.Yaw(), kPi / 6.0f, 1e-4f);
    EXPECT_NEAR(q.Roll(), 0.0f, 1e-4f);
}

TEST(QuaternionTest, EulerRoundTripRoll) {
    const Quaternion<float32> q = Quaternion<float32>::FromEuler(0.0f, 0.0f, kPi / 6.0f);

    EXPECT_NEAR(q.Pitch(), 0.0f, 1e-4f);
    EXPECT_NEAR(q.Yaw(), 0.0f, 1e-4f);
    EXPECT_NEAR(q.Roll(), kPi / 6.0f, 1e-4f);
}

TEST(QuaternionTest, ToEulerMatchesComponentAccessors) {
    const Quaternion<float32> q = Quaternion<float32>::FromEuler(kPi / 8.0f, kPi / 5.0f, kPi / 7.0f);

    const Vec3<float32> euler = q.ToEuler();

    EXPECT_NEAR(euler[0], q.Pitch(), 1e-4f);
    EXPECT_NEAR(euler[1], q.Yaw(), 1e-4f);
    EXPECT_NEAR(euler[2], q.Roll(), 1e-4f);
}

TEST(QuaternionTest, YawClampsAtGimbalLock) {
    const Quaternion<float32> q = Quaternion<float32>::FromEuler(0.0f, kPi / 2.0f, 0.0f);

    EXPECT_NEAR(q.Yaw(), kPi / 2.0f, 1e-3f);
}

// =========================================================================
// Dot / Lerp / Slerp
// =========================================================================

TEST(QuaternionTest, DotOfIdenticalQuaternionsEqualsLengthSquared) {
    constexpr Quaternion<float32> q{1.0f, 2.0f, 3.0f, 4.0f};

    EXPECT_FLOAT_EQ(Dot(q, q), q.LengthSquared());
}

TEST(QuaternionTest, DotOfOrthogonalQuaternionsIsZero) {
    constexpr Quaternion<float32> a{1.0f, 0.0f, 0.0f, 0.0f};
    constexpr Quaternion<float32> b{0.0f, 1.0f, 0.0f, 0.0f};

    EXPECT_FLOAT_EQ(Dot(a, b), 0.0f);
}

TEST(QuaternionTest, LerpAtStart) {
    const Quaternion<float32> a = Quaternion<float32>::FromAxisAngle(Vec3<float32>{0.0f, 0.0f, 1.0f}, 0.0f);
    const Quaternion<float32> b = Quaternion<float32>::FromAxisAngle(Vec3<float32>{0.0f, 0.0f, 1.0f}, kPi / 2.0f);

    const Quaternion<float32> result = Lerp(a, b, 0.0f);

    ExpectNear(result, a);
}

TEST(QuaternionTest, LerpAtEnd) {
    const Quaternion<float32> a = Quaternion<float32>::FromAxisAngle(Vec3<float32>{0.0f, 0.0f, 1.0f}, 0.0f);
    const Quaternion<float32> b = Quaternion<float32>::FromAxisAngle(Vec3<float32>{0.0f, 0.0f, 1.0f}, kPi / 2.0f);

    const Quaternion<float32> result = Lerp(a, b, 1.0f);

    ExpectNear(result, b);
}

TEST(QuaternionTest, LerpIsNormalized) {
    const Quaternion<float32> a = Quaternion<float32>::FromAxisAngle(Vec3<float32>{0.0f, 0.0f, 1.0f}, 0.0f);
    const Quaternion<float32> b = Quaternion<float32>::FromAxisAngle(Vec3<float32>{1.0f, 0.0f, 0.0f}, kPi / 2.0f);

    const Quaternion<float32> result = Lerp(a, b, 0.5f);

    EXPECT_NEAR(result.Length(), 1.0f, 1e-4f);
}

TEST(QuaternionTest, SlerpAtStart) {
    const Quaternion<float32> a = Quaternion<float32>::FromAxisAngle(Vec3<float32>{0.0f, 1.0f, 0.0f}, 0.0f);
    const Quaternion<float32> b = Quaternion<float32>::FromAxisAngle(Vec3<float32>{0.0f, 1.0f, 0.0f}, kPi / 2.0f);

    const Quaternion<float32> result = Slerp(a, b, 0.0f);

    ExpectNear(result, a);
}

TEST(QuaternionTest, SlerpAtEnd) {
    const Quaternion<float32> a = Quaternion<float32>::FromAxisAngle(Vec3<float32>{0.0f, 1.0f, 0.0f}, 0.0f);
    const Quaternion<float32> b = Quaternion<float32>::FromAxisAngle(Vec3<float32>{0.0f, 1.0f, 0.0f}, kPi / 2.0f);

    const Quaternion<float32> result = Slerp(a, b, 1.0f);

    ExpectNear(result, b);
}

TEST(QuaternionTest, SlerpAtMidpointMatchesHalfAngle) {
    const Quaternion<float32> a = Quaternion<float32>::FromAxisAngle(Vec3<float32>{0.0f, 1.0f, 0.0f}, 0.0f);
    const Quaternion<float32> b = Quaternion<float32>::FromAxisAngle(Vec3<float32>{0.0f, 1.0f, 0.0f}, kPi / 2.0f);
    const Quaternion<float32> expected =
        Quaternion<float32>::FromAxisAngle(Vec3<float32>{0.0f, 1.0f, 0.0f}, kPi / 4.0f);

    const Quaternion<float32> result = Slerp(a, b, 0.5f);

    ExpectNear(result, expected);
}

TEST(QuaternionTest, SlerpIsNormalized) {
    const Quaternion<float32> a = Quaternion<float32>::FromAxisAngle(Vec3<float32>{0.0f, 0.0f, 1.0f}, 0.0f);
    const Quaternion<float32> b = Quaternion<float32>::FromAxisAngle(Vec3<float32>{1.0f, 0.0f, 0.0f}, kPi / 2.0f);

    const Quaternion<float32> result = Slerp(a, b, 0.3f);

    EXPECT_NEAR(result.Length(), 1.0f, 1e-4f);
}

TEST(QuaternionTest, SlerpTakesShortestPath) {
    const Quaternion<float32> a = Quaternion<float32>::FromAxisAngle(Vec3<float32>{0.0f, 0.0f, 1.0f}, 0.0f);
    Quaternion<float32> b = Quaternion<float32>::FromAxisAngle(Vec3<float32>{0.0f, 0.0f, 1.0f}, kPi / 2.0f);
    b = -b; // Negate to represent the same rotation via the opposite hemisphere.

    const Quaternion<float32> result = Slerp(a, b, 0.5f);
    const Quaternion<float32> expected =
        Quaternion<float32>::FromAxisAngle(Vec3<float32>{0.0f, 0.0f, 1.0f}, kPi / 4.0f);

    ExpectNear(result, expected);
}

TEST(QuaternionTest, SlerpNearIdenticalQuaternionsFallsBackToLerp) {
    const Quaternion<float32> a = Quaternion<float32>::FromAxisAngle(Vec3<float32>{0.0f, 0.0f, 1.0f}, 0.0f);
    const Quaternion<float32> b = Quaternion<float32>::FromAxisAngle(Vec3<float32>{0.0f, 0.0f, 1.0f}, 1e-7f);

    const Quaternion<float32> result = Slerp(a, b, 0.5f);

    EXPECT_NEAR(result.Length(), 1.0f, 1e-4f);
}

// =========================================================================
// Aliases
// =========================================================================

TEST(QuaternionTest, QuaternionAliases) {
    static_assert(std::is_same_v<Quaternionf, Quaternion<float32>>);
    static_assert(std::is_same_v<Quaterniond, Quaternion<float64>>);

    SUCCEED();
}

// =========================================================================
// Common type
// =========================================================================

TEST(QuaternionTest, CommonType) {
    static_assert(std::is_same_v<QuaternionCommon<float32, float64>, Quaternion<float64>>);

    SUCCEED();
}

// =========================================================================
// Constexpr
// =========================================================================

TEST(QuaternionTest, ConstexprConstruction) {
    constexpr Quaternion<float32> q{1.0f, 2.0f, 3.0f, 4.0f};

    static_assert(q.X() == 1.0f);
    static_assert(q.Y() == 2.0f);
    static_assert(q.Z() == 3.0f);
    static_assert(q.W() == 4.0f);

    SUCCEED();
}

TEST(QuaternionTest, ConstexprArithmetic) {
    constexpr Quaternion<float32> a{1.0f, 2.0f, 3.0f, 4.0f};
    constexpr Quaternion<float32> b{4.0f, 5.0f, 6.0f, 7.0f};

    constexpr auto sum = a + b;
    constexpr auto difference = a - b;

    static_assert(sum.X() == 5.0f);
    static_assert(sum.Y() == 7.0f);
    static_assert(sum.Z() == 9.0f);
    static_assert(sum.W() == 11.0f);

    static_assert(difference.X() == -3.0f);
    static_assert(difference.Y() == -3.0f);
    static_assert(difference.Z() == -3.0f);
    static_assert(difference.W() == -3.0f);

    SUCCEED();
}

// =========================================================================
// Formatting
// =========================================================================

TEST(QuaternionTest, Formatting) {
    constexpr Quaternion<float32> q{1.0f, 2.0f, 3.0f, 4.0f};

    EXPECT_EQ(std::format("{}", q), "(1, 2, 3, 4)");
}

TEST(QuaternionTest, FormattingFloatingPoint) {
    constexpr Quaternion<float32> q{1.0f, 2.5f, 3.25f, 4.0f};

    EXPECT_EQ(std::format("{}", q), "(1, 2.5, 3.25, 4)");
}
