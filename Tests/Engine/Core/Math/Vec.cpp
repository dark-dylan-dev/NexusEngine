// SPDX-License-Identifier: MIT

#include <gtest/gtest.h>

import NE.Engine.Core.Types;
import NE.Engine.Math.Vec;

using namespace Nexus;

// =========================================================================
// Construction
// =========================================================================

TEST(VecTest, DefaultConstruction) {
    constexpr Vec<int, 3> vec{};

    EXPECT_EQ(vec[0], 0);
    EXPECT_EQ(vec[1], 0);
    EXPECT_EQ(vec[2], 0);
}

TEST(VecTest, ValueConstruction) {
    constexpr Vec<int, 3> vec{1, 2, 3};

    EXPECT_EQ(vec[0], 1);
    EXPECT_EQ(vec[1], 2);
    EXPECT_EQ(vec[2], 3);
}

TEST(VecTest, DifferentDimensions) {
    constexpr Vec<int, 2> vec2{1, 2};
    constexpr Vec<int, 3> vec3{1, 2, 3};
    constexpr Vec<int, 4> vec4{1, 2, 3, 4};

    EXPECT_EQ(vec2.Size(), 2);
    EXPECT_EQ(vec3.Size(), 3);
    EXPECT_EQ(vec4.Size(), 4);

    EXPECT_EQ(vec2[0], 1);
    EXPECT_EQ(vec2[1], 2);

    EXPECT_EQ(vec3[0], 1);
    EXPECT_EQ(vec3[1], 2);
    EXPECT_EQ(vec3[2], 3);

    EXPECT_EQ(vec4[0], 1);
    EXPECT_EQ(vec4[1], 2);
    EXPECT_EQ(vec4[2], 3);
    EXPECT_EQ(vec4[3], 4);
}

TEST(VecTest, MixedValueTypes) {
    constexpr Vec<float32, 3> vec{1, 2.5, 3};

    EXPECT_FLOAT_EQ(vec[0], 1.0f);
    EXPECT_FLOAT_EQ(vec[1], 2.5f);
    EXPECT_FLOAT_EQ(vec[2], 3.0f);
}

// =========================================================================
// Access
// =========================================================================

TEST(VecTest, ElementAccess) {
    Vec<int, 3> vec{1, 2, 3};

    vec[0] = 10;
    vec[1] = 20;
    vec[2] = 30;

    EXPECT_EQ(vec[0], 10);
    EXPECT_EQ(vec[1], 20);
    EXPECT_EQ(vec[2], 30);
}

TEST(VecTest, ConstElementAccess) {
    constexpr Vec<int, 3> vec{1, 2, 3};

    EXPECT_EQ(vec[0], 1);
    EXPECT_EQ(vec[1], 2);
    EXPECT_EQ(vec[2], 3);
}

TEST(VecTest, Data) {
    Vec<int, 3> vec{1, 2, 3};

    int* data = vec.Data();

    EXPECT_NE(data, nullptr);

    data[0] = 10;
    data[1] = 20;
    data[2] = 30;

    EXPECT_EQ(vec[0], 10);
    EXPECT_EQ(vec[1], 20);
    EXPECT_EQ(vec[2], 30);
}

TEST(VecTest, Size) {
    EXPECT_EQ((Vec<int, 2>::Size()), 2);
    EXPECT_EQ((Vec<int, 3>::Size()), 3);
    EXPECT_EQ((Vec<int, 4>::Size()), 4);
    EXPECT_EQ((Vec<int, 8>::Size()), 8);
}

// =========================================================================
// Equality
// =========================================================================

TEST(VecTest, Equality) {
    constexpr Vec<int, 3> a{1, 2, 3};
    constexpr Vec<int, 3> b{1, 2, 3};

    EXPECT_TRUE(a == b);
}

TEST(VecTest, Inequality) {
    constexpr Vec<int, 3> a{1, 2, 3};
    constexpr Vec<int, 3> b{1, 2, 4};

    EXPECT_FALSE(a == b);
}

TEST(VecTest, EqualityDifferentTypes) {
    constexpr Vec<int, 3> a{1, 2, 3};
    constexpr Vec<float32, 3> b{1.0f, 2.0f, 3.0f};

    EXPECT_TRUE(a == b);
}

// =========================================================================
// Addition
// =========================================================================

TEST(VecTest, Addition) {
    constexpr Vec<int, 3> a{1, 2, 3};
    constexpr Vec<int, 3> b{4, 5, 6};

    constexpr auto result = a + b;

    EXPECT_EQ(result[0], 5);
    EXPECT_EQ(result[1], 7);
    EXPECT_EQ(result[2], 9);
}

TEST(VecTest, AdditionCommonType) {
    constexpr Vec<int, 3> a{1, 2, 3};
    constexpr Vec<float32, 3> b{0.5f, 1.5f, 2.5f};

    constexpr auto result = a + b;

    static_assert(std::is_same_v<decltype(result), const Vec<float32, 3>>);

    EXPECT_FLOAT_EQ(result[0], 1.5f);
    EXPECT_FLOAT_EQ(result[1], 3.5f);
    EXPECT_FLOAT_EQ(result[2], 5.5f);
}

TEST(VecTest, AdditionAssignment) {
    Vec<int, 3> a{1, 2, 3};
    constexpr Vec<int, 3> b{4, 5, 6};

    a += b;

    EXPECT_EQ(a[0], 5);
    EXPECT_EQ(a[1], 7);
    EXPECT_EQ(a[2], 9);
}

TEST(VecTest, MixedTypeAdditionAssignment) {
    Vec<float32, 3> a{1.0f, 2.0f, 3.0f};
    constexpr Vec<int, 3> b{4, 5, 6};

    a += b;

    EXPECT_FLOAT_EQ(a[0], 5.0f);
    EXPECT_FLOAT_EQ(a[1], 7.0f);
    EXPECT_FLOAT_EQ(a[2], 9.0f);
}

// =========================================================================
// Subtraction
// =========================================================================

TEST(VecTest, Subtraction) {
    constexpr Vec<int, 3> a{5, 7, 9};
    constexpr Vec<int, 3> b{1, 2, 3};

    constexpr auto result = a - b;

    EXPECT_EQ(result[0], 4);
    EXPECT_EQ(result[1], 5);
    EXPECT_EQ(result[2], 6);
}

TEST(VecTest, SubtractionCommonType) {
    constexpr Vec<int, 3> a{5, 7, 9};
    constexpr Vec<float32, 3> b{0.5f, 1.5f, 2.5f};

    constexpr auto result = a - b;

    static_assert(std::is_same_v<decltype(result), const Vec<float32, 3>>);

    EXPECT_FLOAT_EQ(result[0], 4.5f);
    EXPECT_FLOAT_EQ(result[1], 5.5f);
    EXPECT_FLOAT_EQ(result[2], 6.5f);
}

TEST(VecTest, SubtractionAssignment) {
    Vec<int, 3> a{5, 7, 9};
    constexpr Vec<int, 3> b{1, 2, 3};

    a -= b;

    EXPECT_EQ(a[0], 4);
    EXPECT_EQ(a[1], 5);
    EXPECT_EQ(a[2], 6);
}

// =========================================================================
// Component-wise multiplication
// =========================================================================

TEST(VecTest, ComponentMultiplication) {
    constexpr Vec<int, 3> a{1, 2, 3};
    constexpr Vec<int, 3> b{4, 5, 6};

    constexpr auto result = a * b;

    EXPECT_EQ(result[0], 4);
    EXPECT_EQ(result[1], 10);
    EXPECT_EQ(result[2], 18);
}

TEST(VecTest, ComponentMultiplicationCommonType) {
    constexpr Vec<int, 3> a{2, 3, 4};
    constexpr Vec<float32, 3> b{0.5f, 1.5f, 2.5f};

    constexpr auto result = a * b;

    static_assert(std::is_same_v<decltype(result), const Vec<float32, 3>>);

    EXPECT_FLOAT_EQ(result[0], 1.0f);
    EXPECT_FLOAT_EQ(result[1], 4.5f);
    EXPECT_FLOAT_EQ(result[2], 10.0f);
}

TEST(VecTest, ComponentMultiplicationAssignment) {
    Vec<int, 3> a{2, 3, 4};
    constexpr Vec<int, 3> b{5, 6, 7};

    a *= b;

    EXPECT_EQ(a[0], 10);
    EXPECT_EQ(a[1], 18);
    EXPECT_EQ(a[2], 28);
}

// =========================================================================
// Component-wise division
// =========================================================================

TEST(VecTest, ComponentDivision) {
    constexpr Vec<int, 3> a{10, 20, 30};
    constexpr Vec<int, 3> b{2, 4, 5};

    constexpr auto result = a / b;

    EXPECT_EQ(result[0], 5);
    EXPECT_EQ(result[1], 5);
    EXPECT_EQ(result[2], 6);
}

TEST(VecTest, ComponentDivisionCommonType) {
    constexpr Vec<int, 3> a{1, 3, 5};
    constexpr Vec<float32, 3> b{2.0f, 2.0f, 2.0f};

    constexpr auto result = a / b;

    static_assert(std::is_same_v<decltype(result), const Vec<float32, 3>>);

    EXPECT_FLOAT_EQ(result[0], 0.5f);
    EXPECT_FLOAT_EQ(result[1], 1.5f);
    EXPECT_FLOAT_EQ(result[2], 2.5f);
}

TEST(VecTest, ComponentDivisionAssignment) {
    Vec<int, 3> a{10, 20, 30};
    constexpr Vec<int, 3> b{2, 4, 5};

    a /= b;

    EXPECT_EQ(a[0], 5);
    EXPECT_EQ(a[1], 5);
    EXPECT_EQ(a[2], 6);
}

// =========================================================================
// Scalar multiplication
// =========================================================================

TEST(VecTest, ScalarMultiplication) {
    constexpr Vec<int, 3> vec{1, 2, 3};

    constexpr auto result = vec * 2;

    EXPECT_EQ(result[0], 2);
    EXPECT_EQ(result[1], 4);
    EXPECT_EQ(result[2], 6);
}

TEST(VecTest, ScalarMultiplicationCommonType) {
    constexpr Vec<int, 3> vec{1, 2, 3};

    constexpr auto result = vec * 0.5f;

    static_assert(std::is_same_v<decltype(result), const Vec<float32, 3>>);

    EXPECT_FLOAT_EQ(result[0], 0.5f);
    EXPECT_FLOAT_EQ(result[1], 1.0f);
    EXPECT_FLOAT_EQ(result[2], 1.5f);
}

TEST(VecTest, ScalarMultiplicationReversed) {
    constexpr Vec<int, 3> vec{1, 2, 3};

    constexpr auto result = 2 * vec;

    EXPECT_EQ(result[0], 2);
    EXPECT_EQ(result[1], 4);
    EXPECT_EQ(result[2], 6);
}

TEST(VecTest, ScalarMultiplicationAssignment) {
    Vec<int, 3> vec{1, 2, 3};

    vec *= 3;

    EXPECT_EQ(vec[0], 3);
    EXPECT_EQ(vec[1], 6);
    EXPECT_EQ(vec[2], 9);
}

// =========================================================================
// Scalar division
// =========================================================================

TEST(VecTest, ScalarDivision) {
    constexpr Vec<int, 3> vec{10, 20, 30};

    constexpr auto result = vec / 2;

    EXPECT_EQ(result[0], 5);
    EXPECT_EQ(result[1], 10);
    EXPECT_EQ(result[2], 15);
}

TEST(VecTest, ScalarDivisionCommonType) {
    constexpr Vec<int, 3> vec{1, 2, 3};

    constexpr auto result = vec / 2.0f;

    static_assert(std::is_same_v<decltype(result), const Vec<float32, 3>>);

    EXPECT_FLOAT_EQ(result[0], 0.5f);
    EXPECT_FLOAT_EQ(result[1], 1.0f);
    EXPECT_FLOAT_EQ(result[2], 1.5f);
}

TEST(VecTest, ScalarDivisionAssignment) {
    Vec<int, 3> vec{10, 20, 30};

    vec /= 2;

    EXPECT_EQ(vec[0], 5);
    EXPECT_EQ(vec[1], 10);
    EXPECT_EQ(vec[2], 15);
}

// =========================================================================
// Unary operators
// =========================================================================

TEST(VecTest, UnaryPlus) {
    constexpr Vec<int, 3> vec{1, 2, 3};

    constexpr auto result = +vec;

    EXPECT_EQ(result[0], 1);
    EXPECT_EQ(result[1], 2);
    EXPECT_EQ(result[2], 3);
}

TEST(VecTest, UnaryMinus) {
    constexpr Vec<int, 3> vec{1, -2, 3};

    constexpr auto result = -vec;

    EXPECT_EQ(result[0], -1);
    EXPECT_EQ(result[1], 2);
    EXPECT_EQ(result[2], -3);
}

// =========================================================================
// Aliases
// =========================================================================

TEST(VecTest, VecAliases) {
    static_assert(std::is_same_v<Vec2<int>, Vec<int, 2>>);
    static_assert(std::is_same_v<Vec3<int>, Vec<int, 3>>);
    static_assert(std::is_same_v<Vec4<int>, Vec<int, 4>>);

    static_assert(std::is_same_v<Vec2f, Vec<float32, 2>>);
    static_assert(std::is_same_v<Vec3f, Vec<float32, 3>>);
    static_assert(std::is_same_v<Vec4f, Vec<float32, 4>>);

    SUCCEED();
}

// =========================================================================
// Common type
// =========================================================================

TEST(VecTest, CommonType) {
    static_assert(std::is_same_v<VecCommon<int, float32, 2>, Vec<float32, 2>>);

    static_assert(std::is_same_v<VecCommon<float32, float64, 3>, Vec<float64, 3>>);

    SUCCEED();
}

// =========================================================================
// Constexpr
// =========================================================================

TEST(VecTest, ConstexprConstruction) {
    constexpr Vec<int, 3> vec{1, 2, 3};

    static_assert(vec[0] == 1);
    static_assert(vec[1] == 2);
    static_assert(vec[2] == 3);

    SUCCEED();
}

TEST(VecTest, ConstexprArithmetic) {
    constexpr Vec<int, 3> a{1, 2, 3};
    constexpr Vec<int, 3> b{4, 5, 6};

    constexpr auto sum = a + b;
    constexpr auto difference = a - b;
    constexpr auto product = a * b;

    static_assert(sum[0] == 5);
    static_assert(sum[1] == 7);
    static_assert(sum[2] == 9);

    static_assert(difference[0] == -3);
    static_assert(difference[1] == -3);
    static_assert(difference[2] == -3);

    static_assert(product[0] == 4);
    static_assert(product[1] == 10);
    static_assert(product[2] == 18);

    SUCCEED();
}

// =========================================================================
// Higher dimensions
// =========================================================================

TEST(VecTest, HigherDimension) {
    Vec<int, 8> vec{1, 2, 3, 4, 5, 6, 7, 8};

    for (std::size_t i = 0; i < 8; ++i) {
        EXPECT_EQ(vec[i], static_cast<int>(i + 1));
    }
}

// =========================================================================
// Formatting
// =========================================================================

TEST(VecTest, Formatting) {
    constexpr Vec<int, 3> vec{1, 2, 3};

    EXPECT_EQ(std::format("{}", vec), "(1, 2, 3)");
}

TEST(VecTest, FormattingFloatingPoint) {
    constexpr Vec<float32, 4> vec{1.0f, 2.5f, 3.25f, 4.0f};

    EXPECT_EQ(std::format("{}", vec), "(1, 2.5, 3.25, 4)");
}
