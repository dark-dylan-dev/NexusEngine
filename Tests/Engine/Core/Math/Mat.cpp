// SPDX-License-Identifier: MIT

#include <gtest/gtest.h>

import NE.Engine.Core.Types;
import NE.Engine.Math.Mat;
import NE.Engine.Math.Vec;
import NE.Engine.Math.Quaternion;

import std;

using namespace Nexus;

namespace {
    constexpr float32 kPi = std::numbers::pi_v<float32>;

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

TEST(MatTest, DefaultConstruction) {
    constexpr Mat<int, 2, 3> mat;

    for (usize row = 0; row < 2; ++row) {
        for (usize col = 0; col < 3; ++col) {
            EXPECT_EQ((mat[row, col]), 0);
        }
    }
}

TEST(MatTest, ValueConstruction) {
    constexpr Mat<int, 2, 3> mat{1, 2, 3, 4, 5, 6};

    EXPECT_EQ((mat[0, 0]), 1);
    EXPECT_EQ((mat[0, 1]), 2);
    EXPECT_EQ((mat[0, 2]), 3);

    EXPECT_EQ((mat[1, 0]), 4);
    EXPECT_EQ((mat[1, 1]), 5);
    EXPECT_EQ((mat[1, 2]), 6);
}

TEST(MatTest, ValueConstructionWithConversion) {
    constexpr Mat<float32, 2, 2> mat{1, 2, 3, 4};

    EXPECT_FLOAT_EQ((mat[0, 0]), 1.0f);
    EXPECT_FLOAT_EQ((mat[0, 1]), 2.0f);
    EXPECT_FLOAT_EQ((mat[1, 0]), 3.0f);
    EXPECT_FLOAT_EQ((mat[1, 1]), 4.0f);
}

// =========================================================================
// Access
// =========================================================================

TEST(MatTest, BracketAccess) {
    Mat<int, 3, 3> mat{};

    mat[1, 2] = 42;

    EXPECT_EQ((mat[1, 2]), 42);
}

TEST(MatTest, ParenthesisAccess) {
    Mat<int, 3, 3> mat{};

    mat(2, 1) = 42;

    EXPECT_EQ((mat(2, 1)), 42);
}

TEST(MatTest, ConstAccess) {
    constexpr Mat<int, 2, 2> mat{1, 2, 3, 4};

    EXPECT_EQ((mat[0, 0]), 1);
    EXPECT_EQ((mat[1, 1]), 4);
}

TEST(MatTest, Data) {
    Mat<int, 2, 3> mat{1, 2, 3, 4, 5, 6};

    EXPECT_NE(mat.Data(), nullptr);

    mat.Data()[0] = 42;

    EXPECT_EQ((mat[0, 0]), 42);
}

// =========================================================================
// Dimensions
// =========================================================================

TEST(MatTest, Dimensions) {
    using Matrix = Mat<float32, 3, 4>;

    EXPECT_EQ(Matrix::Rows(), 3);
    EXPECT_EQ(Matrix::Cols(), 4);
    EXPECT_EQ(Matrix::Size(), 12);
}

TEST(MatTest, DimensionsAreCompileTimeConstants) {
    using Matrix = Mat<float32, 3, 4>;

    static_assert(Matrix::Rows() == 3);
    static_assert(Matrix::Cols() == 4);
    static_assert(Matrix::Size() == 12);
}

// =========================================================================
// MDSpan
// =========================================================================

TEST(MatTest, MDSpan) {
    Mat<int, 2, 3> mat{1, 2, 3, 4, 5, 6};

    const auto span = mat.MDSpan();

    EXPECT_EQ(span.extent(0), 2);
    EXPECT_EQ(span.extent(1), 3);

    EXPECT_EQ((span[0, 0]), 1);
    EXPECT_EQ((span[0, 2]), 3);
    EXPECT_EQ((span[1, 0]), 4);
    EXPECT_EQ((span[1, 2]), 6);

    span[1, 1] = 42;

    EXPECT_EQ((mat[1, 1]), 42);
}

TEST(MatTest, ConstMDSpan) {
    constexpr Mat<int, 2, 2> mat{1, 2, 3, 4};

    const auto span = mat.MDSpan();

    EXPECT_EQ(span.extent(0), 2);
    EXPECT_EQ(span.extent(1), 2);

    EXPECT_EQ((span[0, 0]), 1);
    EXPECT_EQ((span[1, 1]), 4);
}

// =========================================================================
// Equality
// =========================================================================

TEST(MatTest, Equality) {
    constexpr Mat<int, 2, 2> a{1, 2, 3, 4};

    constexpr Mat<int, 2, 2> b{1, 2, 3, 4};

    constexpr Mat<int, 2, 2> c{1, 2, 3, 5};

    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a == c);
}

TEST(MatTest, EqualityDifferentNumericTypes) {
    constexpr Mat<int, 2, 2> a{1, 2, 3, 4};

    constexpr Mat<float32, 2, 2> b{1.0f, 2.0f, 3.0f, 4.0f};

    EXPECT_TRUE(a == b);
}

TEST(MatTest, EqualityDifferentLayouts) {
    constexpr MatRowMajor<int, 2, 2> row{1, 2, 3, 4};

    constexpr MatColumnMajor<int, 2, 2> column{1, 2, 3, 4};

    EXPECT_TRUE(row == column);
}

// =========================================================================
// Addition
// =========================================================================

TEST(MatTest, Addition) {
    constexpr Mat<int, 2, 2> a{1, 2, 3, 4};

    constexpr Mat<int, 2, 2> b{5, 6, 7, 8};

    constexpr auto result = a + b;

    static_assert(std::is_same_v<decltype(result), const Mat<int, 2, 2>>);

    EXPECT_EQ((result[0, 0]), 6);
    EXPECT_EQ((result[0, 1]), 8);
    EXPECT_EQ((result[1, 0]), 10);
    EXPECT_EQ((result[1, 1]), 12);
}

TEST(MatTest, AdditionDifferentNumericTypes) {
    constexpr Mat<int, 2, 2> a{1, 2, 3, 4};

    constexpr Mat<float32, 2, 2> b{0.5f, 1.5f, 2.5f, 3.5f};

    constexpr auto result = a + b;

    static_assert(std::is_same_v<decltype(result), const Mat<float32, 2, 2>>);

    EXPECT_FLOAT_EQ((result[0, 0]), 1.5f);
    EXPECT_FLOAT_EQ((result[0, 1]), 3.5f);
    EXPECT_FLOAT_EQ((result[1, 0]), 5.5f);
    EXPECT_FLOAT_EQ((result[1, 1]), 7.5f);
}

TEST(MatTest, AdditionAssignment) {
    Mat<int, 2, 2> a{1, 2, 3, 4};

    constexpr Mat<int, 2, 2> b{5, 6, 7, 8};

    a += b;

    EXPECT_EQ((a[0, 0]), 6);
    EXPECT_EQ((a[0, 1]), 8);
    EXPECT_EQ((a[1, 0]), 10);
    EXPECT_EQ((a[1, 1]), 12);
}

// =========================================================================
// Subtraction
// =========================================================================

TEST(MatTest, Subtraction) {
    constexpr Mat<int, 2, 2> a{5, 6, 7, 8};

    constexpr Mat<int, 2, 2> b{1, 2, 3, 4};

    constexpr auto result = a - b;

    EXPECT_EQ((result[0, 0]), 4);
    EXPECT_EQ((result[0, 1]), 4);
    EXPECT_EQ((result[1, 0]), 4);
    EXPECT_EQ((result[1, 1]), 4);
}

TEST(MatTest, SubtractionAssignment) {
    Mat<int, 2, 2> a{5, 6, 7, 8};

    constexpr Mat<int, 2, 2> b{1, 2, 3, 4};

    a -= b;

    EXPECT_EQ((a[0, 0]), 4);
    EXPECT_EQ((a[0, 1]), 4);
    EXPECT_EQ((a[1, 0]), 4);
    EXPECT_EQ((a[1, 1]), 4);
}

// =========================================================================
// Unary operators
// =========================================================================

TEST(MatTest, UnaryPlus) {
    constexpr Mat<int, 2, 2> a{1, 2, 3, 4};

    constexpr auto result = +a;

    EXPECT_EQ(result, a);
}

TEST(MatTest, UnaryMinus) {
    constexpr Mat<int, 2, 2> a{1, -2, 3, -4};

    constexpr auto result = -a;

    EXPECT_EQ((result[0, 0]), -1);
    EXPECT_EQ((result[0, 1]), 2);
    EXPECT_EQ((result[1, 0]), -3);
    EXPECT_EQ((result[1, 1]), 4);
}

// =========================================================================
// Scalar multiplication
// =========================================================================

TEST(MatTest, ScalarMultiplication) {
    constexpr Mat<int, 2, 2> mat{1, 2, 3, 4};

    constexpr auto result = mat * 2;

    EXPECT_EQ((result[0, 0]), 2);
    EXPECT_EQ((result[0, 1]), 4);
    EXPECT_EQ((result[1, 0]), 6);
    EXPECT_EQ((result[1, 1]), 8);
}

TEST(MatTest, ScalarMultiplicationReversed) {
    constexpr Mat<int, 2, 2> mat{1, 2, 3, 4};

    constexpr auto result = 2 * mat;

    EXPECT_EQ((result[0, 0]), 2);
    EXPECT_EQ((result[0, 1]), 4);
    EXPECT_EQ((result[1, 0]), 6);
    EXPECT_EQ((result[1, 1]), 8);
}

TEST(MatTest, ScalarMultiplicationDifferentType) {
    constexpr Mat<int, 2, 2> mat{1, 2, 3, 4};

    constexpr auto result = mat * 0.5f;

    static_assert(std::is_same_v<decltype(result), const Mat<float32, 2, 2>>);

    EXPECT_FLOAT_EQ((result[0, 0]), 0.5f);
    EXPECT_FLOAT_EQ((result[0, 1]), 1.0f);
    EXPECT_FLOAT_EQ((result[1, 0]), 1.5f);
    EXPECT_FLOAT_EQ((result[1, 1]), 2.0f);
}

TEST(MatTest, ScalarMultiplicationAssignment) {
    Mat<int, 2, 2> mat{1, 2, 3, 4};

    mat *= 2;

    EXPECT_EQ((mat[0, 0]), 2);
    EXPECT_EQ((mat[0, 1]), 4);
    EXPECT_EQ((mat[1, 0]), 6);
    EXPECT_EQ((mat[1, 1]), 8);
}

// =========================================================================
// Scalar division
// =========================================================================

TEST(MatTest, ScalarDivision) {
    constexpr Mat<float32, 2, 2> mat{2, 4, 6, 8};

    constexpr auto result = mat / 2.0f;

    EXPECT_FLOAT_EQ((result[0, 0]), 1.0f);
    EXPECT_FLOAT_EQ((result[0, 1]), 2.0f);
    EXPECT_FLOAT_EQ((result[1, 0]), 3.0f);
    EXPECT_FLOAT_EQ((result[1, 1]), 4.0f);
}

TEST(MatTest, ScalarDivisionDifferentType) {
    constexpr Mat<int, 2, 2> mat{2, 4, 6, 8};

    constexpr auto result = mat / 2.0f;

    static_assert(std::is_same_v<decltype(result), const Mat<float32, 2, 2>>);

    EXPECT_FLOAT_EQ((result[0, 0]), 1.0f);
    EXPECT_FLOAT_EQ((result[0, 1]), 2.0f);
    EXPECT_FLOAT_EQ((result[1, 0]), 3.0f);
    EXPECT_FLOAT_EQ((result[1, 1]), 4.0f);
}

TEST(MatTest, ScalarDivisionAssignment) {
    Mat<float32, 2, 2> mat{2, 4, 6, 8};

    mat /= 2.0f;

    EXPECT_FLOAT_EQ((mat[0, 0]), 1.0f);
    EXPECT_FLOAT_EQ((mat[0, 1]), 2.0f);
    EXPECT_FLOAT_EQ((mat[1, 0]), 3.0f);
    EXPECT_FLOAT_EQ((mat[1, 1]), 4.0f);
}

// =========================================================================
// Matrix multiplication
// =========================================================================

TEST(MatTest, MatrixMultiplication) {
    constexpr Mat<int, 2, 3> a{1, 2, 3, 4, 5, 6};

    constexpr Mat<int, 3, 2> b{7, 8, 9, 10, 11, 12};

    constexpr auto result = a * b;

    static_assert(std::is_same_v<decltype(result), const Mat<int, 2, 2>>);

    EXPECT_EQ((result[0, 0]), 58);
    EXPECT_EQ((result[0, 1]), 64);
    EXPECT_EQ((result[1, 0]), 139);
    EXPECT_EQ((result[1, 1]), 154);
}

TEST(MatTest, MatrixMultiplicationDifferentNumericTypes) {
    constexpr Mat<int, 2, 3> a{1, 2, 3, 4, 5, 6};

    constexpr Mat<float32, 3, 2> b{1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f};

    constexpr auto result = a * b;

    static_assert(std::is_same_v<decltype(result), const Mat<float32, 2, 2>>);

    EXPECT_FLOAT_EQ((result[0, 0]), 22.0f);
    EXPECT_FLOAT_EQ((result[0, 1]), 28.0f);
    EXPECT_FLOAT_EQ((result[1, 0]), 49.0f);
    EXPECT_FLOAT_EQ((result[1, 1]), 64.0f);
}

TEST(MatTest, SquareMatrixMultiplication) {
    constexpr Mat<int, 2, 2> a{1, 2, 3, 4};

    constexpr Mat<int, 2, 2> b{5, 6, 7, 8};

    constexpr auto result = a * b;

    EXPECT_EQ((result[0, 0]), 19);
    EXPECT_EQ((result[0, 1]), 22);
    EXPECT_EQ((result[1, 0]), 43);
    EXPECT_EQ((result[1, 1]), 50);
}

// =========================================================================
// Transpose
// =========================================================================

TEST(MatTest, Transpose) {
    constexpr Mat<int, 2, 3> mat{1, 2, 3, 4, 5, 6};

    constexpr auto result = mat.Transpose();

    static_assert(std::is_same_v<decltype(result), const Mat<int, 3, 2>>);

    EXPECT_EQ((result[0, 0]), 1);
    EXPECT_EQ((result[0, 1]), 4);

    EXPECT_EQ((result[1, 0]), 2);
    EXPECT_EQ((result[1, 1]), 5);

    EXPECT_EQ((result[2, 0]), 3);
    EXPECT_EQ((result[2, 1]), 6);
}

TEST(MatTest, SquareTranspose) {
    constexpr Mat<int, 3, 3> mat{1, 2, 3, 4, 5, 6, 7, 8, 9};

    constexpr auto result = mat.Transpose();

    EXPECT_EQ((result[0, 0]), 1);
    EXPECT_EQ((result[0, 1]), 4);
    EXPECT_EQ((result[0, 2]), 7);

    EXPECT_EQ((result[1, 0]), 2);
    EXPECT_EQ((result[1, 1]), 5);
    EXPECT_EQ((result[1, 2]), 8);

    EXPECT_EQ((result[2, 0]), 3);
    EXPECT_EQ((result[2, 1]), 6);
    EXPECT_EQ((result[2, 2]), 9);
}

// =========================================================================
// Identity
// =========================================================================

TEST(MatTest, Identity2x2) {
    constexpr auto identity = SquareMatrix<int, 2>::Identity();

    EXPECT_EQ((identity[0, 0]), 1);
    EXPECT_EQ((identity[0, 1]), 0);
    EXPECT_EQ((identity[1, 0]), 0);
    EXPECT_EQ((identity[1, 1]), 1);
}

TEST(MatTest, Identity4x4) {
    constexpr auto identity = SquareMatrix<float32, 4>::Identity();

    for (usize row = 0; row < 4; ++row) {
        for (usize col = 0; col < 4; ++col) {
            if (row == col) {
                EXPECT_FLOAT_EQ((identity[row, col]), 1.0f);
            } else {
                EXPECT_FLOAT_EQ((identity[row, col]), 0.0f);
            }
        }
    }
}

TEST(MatTest, IdentityIsNeutralForMultiplication) {
    constexpr Mat<int, 3, 3> mat{1, 2, 3, 4, 5, 6, 7, 8, 9};

    constexpr auto identity = SquareMatrix<int, 3>::Identity();

    EXPECT_TRUE(mat * identity == mat);
    EXPECT_TRUE(identity * mat == mat);
}

// =========================================================================
// Determinant
// =========================================================================

TEST(MatTest, Determinant1x1) {
    constexpr Mat<float32, 1, 1> mat{5.0f};

    EXPECT_FLOAT_EQ(mat.Determinant(), 5.0f);
}

TEST(MatTest, Determinant2x2) {
    constexpr Mat<float32, 2, 2> mat{1, 2, 3, 4};

    EXPECT_FLOAT_EQ(mat.Determinant(), -2.0f);
}

TEST(MatTest, Determinant3x3) {
    constexpr Mat<float32, 3, 3> mat{6, 1, 1, 4, -2, 5, 2, 8, 7};

    EXPECT_FLOAT_EQ(mat.Determinant(), -306.0f);
}

TEST(MatTest, DeterminantIdentity) {
    constexpr auto identity = SquareMatrix<float32, 4>::Identity();

    EXPECT_FLOAT_EQ(identity.Determinant(), 1.0f);
}

TEST(MatTest, DeterminantSingular) {
    constexpr Mat<float32, 2, 2> mat{1, 2, 2, 4};

    EXPECT_FLOAT_EQ(mat.Determinant(), 0.0f);
}

// =========================================================================
// Inverse
// =========================================================================

TEST(MatTest, Inverse2x2) {
    constexpr Mat<float64, 2, 2> mat{4.0, 7.0, 2.0, 6.0};

    constexpr auto inverse = mat.Inverse();

    EXPECT_NEAR((inverse[0, 0]), 0.6, 1e-9);
    EXPECT_NEAR((inverse[0, 1]), -0.7, 1e-9);
    EXPECT_NEAR((inverse[1, 0]), -0.2, 1e-9);
    EXPECT_NEAR((inverse[1, 1]), 0.4, 1e-9);
}

TEST(MatTest, InverseIdentity) {
    constexpr auto identity = SquareMatrix<float64, 4>::Identity();

    constexpr auto inverse = identity.Inverse();

    EXPECT_TRUE(inverse == identity);
}

TEST(MatTest, MatrixTimesInverseIsIdentity) {
    constexpr Mat<float64, 2, 2> mat{4.0, 7.0, 2.0, 6.0};

    constexpr auto inverse = mat.Inverse();
    constexpr auto result = mat * inverse;

    constexpr auto identity = SquareMatrix<float64, 2>::Identity();

    for (usize row = 0; row < 2; ++row) {
        for (usize col = 0; col < 2; ++col) {
            EXPECT_NEAR((result[row, col]), (identity[row, col]), 1e-9);
        }
    }
}

TEST(MatTest, SingularMatrixInverse) {
    constexpr Mat<float64, 2, 2> mat{1.0, 2.0, 2.0, 4.0};

    constexpr auto inverse = mat.Inverse();
    constexpr auto b = Mat<float64, 2, 2>{};

    EXPECT_TRUE(inverse == b);
}

// =========================================================================
// Zero
// =========================================================================

TEST(MatTest, Zero) {
    constexpr auto mat = Mat4f::Zero();

    for (usize row = 0; row < 4; ++row) {
        for (usize col = 0; col < 4; ++col) {
            EXPECT_FLOAT_EQ((mat[row, col]), 0.0f);
        }
    }
}

// =========================================================================
// Rotation factories
// =========================================================================

TEST(MatTest, RotationXRotatesYIntoZ) {
    const Mat4f rot = Mat4f::RotationX(kPi / 2.0f);
    constexpr Vec3f v{0.0f, 1.0f, 0.0f};

    const Vec3f result = rot.MultiplyVector(v);

    ExpectNear(result, Vec3f{0.0f, 0.0f, 1.0f});
}

TEST(MatTest, RotationYRotatesZIntoX) {
    const Mat4f rot = Mat4f::RotationY(kPi / 2.0f);
    constexpr Vec3f v{0.0f, 0.0f, 1.0f};

    const Vec3f result = rot.MultiplyVector(v);

    ExpectNear(result, Vec3f{1.0f, 0.0f, 0.0f});
}

TEST(MatTest, RotationZRotatesXIntoY) {
    const Mat4f rot = Mat4f::RotationZ(kPi / 2.0f);
    constexpr Vec3f v{1.0f, 0.0f, 0.0f};

    const Vec3f result = rot.MultiplyVector(v);

    ExpectNear(result, Vec3f{0.0f, 1.0f, 0.0f});
}

TEST(MatTest, RotationXByZeroIsIdentity) {
    const Mat4f rot = Mat4f::RotationX(0.0f);
    const Mat4f identity = Mat4f::Identity();

    for (usize row = 0; row < 4; ++row) {
        for (usize col = 0; col < 4; ++col) {
            EXPECT_NEAR((rot[row, col]), (identity[row, col]), 1e-6f);
        }
    }
}

// =========================================================================
// Scale / Translate factories
// =========================================================================

TEST(MatTest, ScaleFactoryScalesPoint) {
    const Mat4f mat = Mat4f::Scale(Vec3f{2.0f, 3.0f, 4.0f});
    constexpr Vec3f point{1.0f, 1.0f, 1.0f};

    const Vec3f result = mat.MultiplyPoint(point);

    ExpectNear(result, Vec3f{2.0f, 3.0f, 4.0f});
}

TEST(MatTest, TranslateFactoryTranslatesPoint) {
    const Mat4f mat = Mat4f::Translate(Vec3f{1.0f, 2.0f, 3.0f});
    constexpr Vec3f point{0.0f, 0.0f, 0.0f};

    const Vec3f result = mat.MultiplyPoint(point);

    ExpectNear(result, Vec3f{1.0f, 2.0f, 3.0f});
}

TEST(MatTest, TranslateFactoryDoesNotAffectVector) {
    const Mat4f mat = Mat4f::Translate(Vec3f{1.0f, 2.0f, 3.0f});
    constexpr Vec3f direction{5.0f, 0.0f, 0.0f};

    const Vec3f result = mat.MultiplyVector(direction);

    ExpectNear(result, direction);
}

// =========================================================================
// Rotate (from quaternion)
// =========================================================================

TEST(MatTest, RotateMatchesQuaternionRotateVector) {
    const Quaternionf rotation = Quaternionf::FromAxisAngle(Vec3f{0.0f, 1.0f, 0.0f}, kPi / 3.0f);
    const Mat4f mat = Mat4f::Rotate(rotation);

    constexpr Vec3f v{1.0f, 0.0f, 0.0f};

    ExpectNear(mat.MultiplyVector(v), rotation.RotateVector(v));
}

TEST(MatTest, RotateIdentityQuaternionIsIdentityMatrix) {
    const Mat4f mat = Mat4f::Rotate(Quaternionf::Identity());
    const Mat4f identity = Mat4f::Identity();

    for (usize row = 0; row < 4; ++row) {
        for (usize col = 0; col < 4; ++col) {
            EXPECT_NEAR((mat[row, col]), (identity[row, col]), 1e-6f);
        }
    }
}

// =========================================================================
// TRS
// =========================================================================

TEST(MatTest, TRSAppliesScaleRotateTranslateInOrder) {
    constexpr Vec3f translation{10.0f, 0.0f, 0.0f};
    const Quaternionf rotation = Quaternionf::Identity();
    constexpr Vec3f scale{2.0f, 1.0f, 1.0f};

    const Mat4f trs = Mat4f::TRS(translation, rotation, scale);
    constexpr Vec3f localPoint{1.0f, 0.0f, 0.0f};

    const Vec3f worldPoint = trs.MultiplyPoint(localPoint);

    ExpectNear(worldPoint, Vec3f{12.0f, 0.0f, 0.0f});
}

TEST(MatTest, TRSWithIdentityComponentsIsIdentity) {
    const Mat4f trs = Mat4f::TRS(Vec3f{0.0f, 0.0f, 0.0f}, Quaternionf::Identity(), Vec3f{1.0f, 1.0f, 1.0f});
    const Mat4f identity = Mat4f::Identity();

    for (usize row = 0; row < 4; ++row) {
        for (usize col = 0; col < 4; ++col) {
            EXPECT_NEAR((trs[row, col]), (identity[row, col]), 1e-6f);
        }
    }
}

// =========================================================================
// Perspective / Ortho / LookAt
// =========================================================================

TEST(MatTest, PerspectiveMapsNearPlaneToNegativeOneNDC) {
    const Mat4f persp = Mat4f::Perspective(kPi / 2.0f, 1.0f, 0.1f, 100.0f);

    const Vec4f nearPoint{0.0f, 0.0f, -0.1f, 1.0f};
    const Vec4f clip = persp * nearPoint;

    EXPECT_NEAR(clip[2] / clip[3], -1.0f, 1e-4f);
}

TEST(MatTest, PerspectiveMapsFarPlaneToPositiveOneNDC) {
    const Mat4f persp = Mat4f::Perspective(kPi / 2.0f, 1.0f, 0.1f, 100.0f);

    const Vec4f farPoint{0.0f, 0.0f, -100.0f, 1.0f};
    const Vec4f clip = persp * farPoint;

    EXPECT_NEAR(clip[2] / clip[3], 1.0f, 1e-4f);
}

TEST(MatTest, OrthoMapsNearPlaneToNegativeOne) {
    const Mat4f ortho = Mat4f::Ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f);

    const Vec3f nearPoint{0.0f, 0.0f, -0.1f};
    const Vec3f mapped = ortho.MultiplyPoint(nearPoint);

    EXPECT_NEAR(mapped[2], -1.0f, 1e-4f);
}

TEST(MatTest, OrthoMapsFarPlaneToPositiveOne) {
    const Mat4f ortho = Mat4f::Ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f);

    const Vec3f farPoint{0.0f, 0.0f, -100.0f};
    const Vec3f mapped = ortho.MultiplyPoint(farPoint);

    EXPECT_NEAR(mapped[2], 1.0f, 1e-4f);
}

TEST(MatTest, OrthoMapsSideBoundsToUnitRange) {
    const Mat4f ortho = Mat4f::Ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f);

    const Vec3f corner{1.0f, 1.0f, -0.1f};
    const Vec3f mapped = ortho.MultiplyPoint(corner);

    EXPECT_NEAR(mapped[0], 1.0f, 1e-4f);
    EXPECT_NEAR(mapped[1], 1.0f, 1e-4f);
}

TEST(MatTest, LookAtMapsEyeToOrigin) {
    constexpr Vec3f eye{0.0f, 0.0f, 5.0f};
    constexpr Vec3f target{0.0f, 0.0f, 0.0f};
    constexpr Vec3f up{0.0f, 1.0f, 0.0f};

    const Mat4f lookAt = Mat4f::LookAt(eye, target, up);
    const Vec3f eyeInView = lookAt.MultiplyPoint(eye);

    ExpectNear(eyeInView, Vec3f{0.0f, 0.0f, 0.0f});
}

TEST(MatTest, LookAtRotationIsOrthonormal) {
    constexpr Vec3f eye{0.0f, 0.0f, 5.0f};
    constexpr Vec3f target{0.0f, 0.0f, 0.0f};
    constexpr Vec3f up{0.0f, 1.0f, 0.0f};

    const Mat4f lookAt = Mat4f::LookAt(eye, target, up);

    const Vec3f r0{lookAt[0, 0], lookAt[0, 1], lookAt[0, 2]};
    const Vec3f r1{lookAt[1, 0], lookAt[1, 1], lookAt[1, 2]};
    const Vec3f r2{lookAt[2, 0], lookAt[2, 1], lookAt[2, 2]};

    EXPECT_NEAR(r0.Length(), 1.0f, 1e-4f);
    EXPECT_NEAR(r1.Length(), 1.0f, 1e-4f);
    EXPECT_NEAR(r2.Length(), 1.0f, 1e-4f);

    EXPECT_NEAR(Dot(r0, r1), 0.0f, 1e-4f);
    EXPECT_NEAR(Dot(r0, r2), 0.0f, 1e-4f);
    EXPECT_NEAR(Dot(r1, r2), 0.0f, 1e-4f);
}

// =========================================================================
// MultiplyPoint / MultiplyVector
// =========================================================================

TEST(MatTest, MultiplyPointAppliesTranslation) {
    const Mat4f mat = Mat4f::Translate(Vec3f{1.0f, 2.0f, 3.0f});
    constexpr Vec3f point{1.0f, 1.0f, 1.0f};

    const Vec3f result = mat.MultiplyPoint(point);

    ExpectNear(result, Vec3f{2.0f, 3.0f, 4.0f});
}

TEST(MatTest, MultiplyVectorIgnoresTranslation) {
    const Mat4f mat = Mat4f::Translate(Vec3f{1.0f, 2.0f, 3.0f});
    constexpr Vec3f vector{1.0f, 1.0f, 1.0f};

    const Vec3f result = mat.MultiplyVector(vector);

    ExpectNear(result, vector);
}

// =========================================================================
// Extract Position / Scale / Rotation
// =========================================================================

TEST(MatTest, ExtractPositionFromTRS) {
    constexpr Vec3f translation{1.0f, 2.0f, 3.0f};
    const Mat4f trs = Mat4f::TRS(translation, Quaternionf::Identity(), Vec3f{1.0f, 1.0f, 1.0f});

    ExpectNear(trs.ExtractPosition(), translation);
}

TEST(MatTest, ExtractScaleFromTRS) {
    constexpr Vec3f scale{2.0f, 3.0f, 4.0f};
    const Mat4f trs = Mat4f::TRS(Vec3f{0.0f, 0.0f, 0.0f}, Quaternionf::Identity(), scale);

    ExpectNear(trs.ExtractScale(), scale);
}

TEST(MatTest, ExtractRotationFromTRS) {
    const Quaternionf rotation = Quaternionf::FromAxisAngle(Vec3f{0.0f, 1.0f, 0.0f}, kPi / 4.0f);
    const Mat4f trs = Mat4f::TRS(Vec3f{0.0f, 0.0f, 0.0f}, rotation, Vec3f{1.0f, 1.0f, 1.0f});

    const Quaternionf extracted = trs.ExtractRotation();

    EXPECT_NEAR(extracted.X(), rotation.X(), 1e-4f);
    EXPECT_NEAR(extracted.Y(), rotation.Y(), 1e-4f);
    EXPECT_NEAR(extracted.Z(), rotation.Z(), 1e-4f);
    EXPECT_NEAR(extracted.W(), rotation.W(), 1e-4f);
}

TEST(MatTest, ExtractRoundTripsFullTRS) {
    constexpr Vec3f translation{5.0f, -2.0f, 1.0f};
    const Quaternionf rotation = Quaternionf::FromAxisAngle(Vec3f{1.0f, 0.0f, 0.0f}, kPi / 6.0f);
    constexpr Vec3f scale{2.0f, 1.0f, 3.0f};

    const Mat4f trs = Mat4f::TRS(translation, rotation, scale);

    ExpectNear(trs.ExtractPosition(), translation);
    ExpectNear(trs.ExtractScale(), scale);
}

// =========================================================================
// Matrix-vector multiplication
// =========================================================================

TEST(MatTest, IdentityTimesVectorIsUnchanged) {
    const Mat4f identity = Mat4f::Identity();
    constexpr Vec4f v{1.0f, 2.0f, 3.0f, 1.0f};

    const Vec4f result = identity * v;

    EXPECT_FLOAT_EQ(result[0], 1.0f);
    EXPECT_FLOAT_EQ(result[1], 2.0f);
    EXPECT_FLOAT_EQ(result[2], 3.0f);
    EXPECT_FLOAT_EQ(result[3], 1.0f);
}

TEST(MatTest, MatrixTimesVectorAppliesTransform) {
    const Mat4f translate = Mat4f::Translate(Vec3f{1.0f, 2.0f, 3.0f});
    constexpr Vec4f v{0.0f, 0.0f, 0.0f, 1.0f};

    const Vec4f result = translate * v;

    EXPECT_FLOAT_EQ(result[0], 1.0f);
    EXPECT_FLOAT_EQ(result[1], 2.0f);
    EXPECT_FLOAT_EQ(result[2], 3.0f);
    EXPECT_FLOAT_EQ(result[3], 1.0f);
}

// =========================================================================
// ToRadians
// =========================================================================

TEST(MatTest, ToRadiansZero) {
    EXPECT_FLOAT_EQ(ToRadians(0.0f), 0.0f);
}

TEST(MatTest, ToRadiansOneEighty) {
    EXPECT_FLOAT_EQ(ToRadians(180.0f), kPi);
}

TEST(MatTest, ToRadiansNinety) {
    EXPECT_NEAR(ToRadians(90.0f), kPi / 2.0f, 1e-5f);
}

// =========================================================================
// ValidTRS
// =========================================================================

TEST(MatTest, ValidTRSAcceptsIdentity) {
    EXPECT_TRUE(ValidTRS(Mat4f::Identity()));
}

TEST(MatTest, ValidTRSAcceptsProperTRS) {
    const Mat4f trs =
        Mat4f::TRS(Vec3f{1.0f, 2.0f, 3.0f}, Quaternionf::FromAxisAngle(Vec3f{0.0f, 1.0f, 0.0f}, kPi / 4.0f),
                   Vec3f{2.0f, 2.0f, 2.0f});

    EXPECT_TRUE(ValidTRS(trs));
}

TEST(MatTest, ValidTRSRejectsZeroMatrix) {
    EXPECT_FALSE(ValidTRS(Mat4f::Zero()));
}

TEST(MatTest, ValidTRSRejectsNonAffineBottomRow) {
    Mat4f mat = Mat4f::Identity();
    mat[3, 0] = 1.0f;

    EXPECT_FALSE(ValidTRS(mat));
}

// =========================================================================
// Layout
// =========================================================================

TEST(MatTest, RowMajorMemoryLayout) {
    constexpr MatRowMajor<int, 2, 3> mat{1, 2, 3, 4, 5, 6};

    EXPECT_EQ(mat.Data()[0], 1);
    EXPECT_EQ(mat.Data()[1], 2);
    EXPECT_EQ(mat.Data()[2], 3);
    EXPECT_EQ(mat.Data()[3], 4);
    EXPECT_EQ(mat.Data()[4], 5);
    EXPECT_EQ(mat.Data()[5], 6);
}

TEST(MatTest, ColumnMajorMemoryLayout) {
    constexpr MatColumnMajor<int, 2, 3> mat{1, 2, 3, 4, 5, 6};

    EXPECT_EQ(mat.Data()[0], 1);
    EXPECT_EQ(mat.Data()[1], 4);
    EXPECT_EQ(mat.Data()[2], 2);
    EXPECT_EQ(mat.Data()[3], 5);
    EXPECT_EQ(mat.Data()[4], 3);
    EXPECT_EQ(mat.Data()[5], 6);
}

TEST(MatTest, LayoutDoesNotChangeMathematicalIndexing) {
    constexpr MatRowMajor<int, 2, 3> row{1, 2, 3, 4, 5, 6};

    constexpr MatColumnMajor<int, 2, 3> column{1, 2, 3, 4, 5, 6};

    for (usize r = 0; r < 2; ++r) {
        for (usize c = 0; c < 3; ++c) {
            EXPECT_EQ((row[r, c]), (column[r, c]));
        }
    }
}

// =========================================================================
// Layout conversion
// =========================================================================

TEST(MatTest, RowToColumnMajorConversion) {
    constexpr MatRowMajor<int, 2, 3> row{1, 2, 3, 4, 5, 6};

    constexpr auto column = MatCastLayout<int, 2, 3, RowMajor, ColumnMajor>(row);

    EXPECT_EQ((column[0, 0]), 1);
    EXPECT_EQ((column[0, 1]), 2);
    EXPECT_EQ((column[0, 2]), 3);

    EXPECT_EQ((column[1, 0]), 4);
    EXPECT_EQ((column[1, 1]), 5);
    EXPECT_EQ((column[1, 2]), 6);

    EXPECT_EQ(column.Data()[0], 1);
    EXPECT_EQ(column.Data()[1], 4);
    EXPECT_EQ(column.Data()[2], 2);
    EXPECT_EQ(column.Data()[3], 5);
    EXPECT_EQ(column.Data()[4], 3);
    EXPECT_EQ(column.Data()[5], 6);
}

TEST(MatTest, ColumnToRowMajorConversion) {
    constexpr MatColumnMajor<int, 2, 3> column{1, 2, 3, 4, 5, 6};

    constexpr auto row = MatCastLayout<int, 2, 3, ColumnMajor, RowMajor>(column);

    EXPECT_EQ((row[0, 0]), 1);
    EXPECT_EQ((row[0, 1]), 2);
    EXPECT_EQ((row[0, 2]), 3);

    EXPECT_EQ((row[1, 0]), 4);
    EXPECT_EQ((row[1, 1]), 5);
    EXPECT_EQ((row[1, 2]), 6);

    EXPECT_EQ(row.Data()[0], 1);
    EXPECT_EQ(row.Data()[1], 2);
    EXPECT_EQ(row.Data()[2], 3);
    EXPECT_EQ(row.Data()[3], 4);
    EXPECT_EQ(row.Data()[4], 5);
    EXPECT_EQ(row.Data()[5], 6);
}

TEST(MatTest, LayoutConversionConstructor) {
    constexpr MatRowMajor<int, 2, 2> row{1, 2, 3, 4};

    constexpr MatColumnMajor<int, 2, 2> column{row};

    EXPECT_EQ((column[0, 0]), 1);
    EXPECT_EQ((column[0, 1]), 2);
    EXPECT_EQ((column[1, 0]), 3);
    EXPECT_EQ((column[1, 1]), 4);

    EXPECT_EQ(column.Data()[0], 1);
    EXPECT_EQ(column.Data()[1], 3);
    EXPECT_EQ(column.Data()[2], 2);
    EXPECT_EQ(column.Data()[3], 4);
}

// =========================================================================
// Layout-aware operations
// =========================================================================

TEST(MatTest, RowMajorMultiplication) {
    constexpr MatRowMajor<int, 2, 2> a{1, 2, 3, 4};

    constexpr MatRowMajor<int, 2, 2> b{5, 6, 7, 8};

    constexpr auto result = a * b;

    EXPECT_EQ((result[0, 0]), 19);
    EXPECT_EQ((result[0, 1]), 22);
    EXPECT_EQ((result[1, 0]), 43);
    EXPECT_EQ((result[1, 1]), 50);
}

TEST(MatTest, ColumnMajorMultiplication) {
    constexpr MatColumnMajor<int, 2, 2> a{1, 2, 3, 4};

    constexpr MatColumnMajor<int, 2, 2> b{5, 6, 7, 8};

    constexpr auto result = a * b;

    EXPECT_EQ((result[0, 0]), 19);
    EXPECT_EQ((result[0, 1]), 22);
    EXPECT_EQ((result[1, 0]), 43);
    EXPECT_EQ((result[1, 1]), 50);
}

// =========================================================================
// Numeric conversion
// =========================================================================

TEST(MatTest, NumericConversion) {
    constexpr Mat<int, 2, 2> source{1, 2, 3, 4};

    constexpr Mat<float32, 2, 2> result{source};

    EXPECT_FLOAT_EQ((result[0, 0]), 1.0f);
    EXPECT_FLOAT_EQ((result[0, 1]), 2.0f);
    EXPECT_FLOAT_EQ((result[1, 0]), 3.0f);
    EXPECT_FLOAT_EQ((result[1, 1]), 4.0f);
}

TEST(MatTest, NumericAndLayoutConversion) {
    constexpr MatRowMajor<int, 2, 2> source{1, 2, 3, 4};

    constexpr MatColumnMajor<float32, 2, 2> result{source};

    EXPECT_FLOAT_EQ((result[0, 0]), 1.0f);
    EXPECT_FLOAT_EQ((result[0, 1]), 2.0f);
    EXPECT_FLOAT_EQ((result[1, 0]), 3.0f);
    EXPECT_FLOAT_EQ((result[1, 1]), 4.0f);

    EXPECT_FLOAT_EQ(result.Data()[0], 1.0f);
    EXPECT_FLOAT_EQ(result.Data()[1], 3.0f);
    EXPECT_FLOAT_EQ(result.Data()[2], 2.0f);
    EXPECT_FLOAT_EQ(result.Data()[3], 4.0f);
}

// =========================================================================
// Aliases
// =========================================================================

TEST(MatTest, Aliases) {
    static_assert(std::is_same_v<Mat2f, SquareMatrix<float32, 2>>);
    static_assert(std::is_same_v<Mat3f, SquareMatrix<float32, 3>>);
    static_assert(std::is_same_v<Mat4f, SquareMatrix<float32, 4>>);

    static_assert(std::is_same_v<SquareMatrixRowMajor<float32, 4>, Mat<float32, 4, 4, RowMajor>>);
    static_assert(std::is_same_v<SquareMatrixColumnMajor<float32, 4>, Mat<float32, 4, 4, ColumnMajor>>);

    static_assert(std::is_same_v<MatRowMajor<float32, 3, 4>, Mat<float32, 3, 4, RowMajor>>);
    static_assert(std::is_same_v<MatColumnMajor<float32, 3, 4>, Mat<float32, 3, 4, ColumnMajor>>);

    SUCCEED();
}

// =========================================================================
// Formatting
// =========================================================================

TEST(MatTest, Formatting) {
    constexpr Mat<int, 2, 2> mat{1, 2, 3, 4};

    EXPECT_EQ(std::format("{}", mat), "[[1, 2], [3, 4]]");
}

TEST(MatTest, FormattingFloatingPoint) {
    constexpr Mat<float32, 2, 2> mat{1.0f, 2.5f, 3.25f, 4.0f};

    EXPECT_EQ(std::format("{}", mat), "[[1, 2.5], [3.25, 4]]");
}
