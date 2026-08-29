// SPDX-License-Identifier: MIT

module;

#include <Platform/DLL/Export.hpp>

export module NE.Engine.Math.Mat;

import NE.Engine.Core.Config;
import NE.Engine.Core.Types;

import std;

export namespace Nexus {

    using RowMajor = std::layout_right;
    using ColumnMajor = std::layout_left;

    template <Numeric T, usize RowCount, usize ColCount, typename Layout = RowMajor>
    class NEXUS_API Mat {
    public:
        using value_type = T;
        using size_type = usize;
        using layout_type = Layout;

        using extents_type = std::extents<size_type, RowCount, ColCount>;
        using mapping_type = Layout::template mapping<extents_type>;
        using mdspan_type = std::mdspan<T, extents_type, Layout>;
        using const_mdspan_type = std::mdspan<const T, extents_type, Layout>;

    private:
        static constexpr extents_type m_extents{};
        static constexpr mapping_type m_mapping{m_extents};
        std::array<T, RowCount * ColCount> m_data{};

    public:
        constexpr Mat() = default;

        template <typename... Args>
            requires(sizeof...(Args) == RowCount * ColCount) && (std::convertible_to<Args, T> && ...)
        constexpr Mat(Args&&... args);

        template <Numeric U, typename OtherLayout>
        constexpr Mat(const Mat<U, RowCount, ColCount, OtherLayout>& other);

        constexpr T& operator[](size_type row, size_type col);
        constexpr const T& operator[](size_type row, size_type col) const;

        constexpr T& operator()(size_type row, size_type col);
        constexpr const T& operator()(size_type row, size_type col) const;

        constexpr T* Data();
        constexpr const T* Data() const;

        constexpr auto MDSpan();
        constexpr auto MDSpan() const;

        static constexpr size_type Rows();
        static constexpr size_type Cols();
        static constexpr size_type Size();

        template <Numeric U>
        constexpr bool operator==(const Mat<U, RowCount, ColCount, Layout>& other) const;

        template <Numeric U, typename OtherLayout>
        constexpr bool operator==(const Mat<U, RowCount, ColCount, OtherLayout>& other) const;

        template <Numeric U, typename OtherLayout>
        constexpr Mat& operator+=(const Mat<U, RowCount, ColCount, OtherLayout>& other);

        template <Numeric U, typename OtherLayout>
        constexpr Mat& operator-=(const Mat<U, RowCount, ColCount, OtherLayout>& other);

        template <Numeric U>
        constexpr Mat& operator*=(U scalar);

        template <Numeric U>
        constexpr Mat& operator/=(U scalar);

        constexpr Mat operator+() const;
        constexpr Mat operator-() const;

        constexpr Mat<T, ColCount, RowCount, Layout> Transpose() const;

        template <usize N = RowCount>
            requires(RowCount == ColCount && N == RowCount)
        static constexpr Mat Identity();

        template <usize N = RowCount>
            requires(RowCount == ColCount && N == RowCount)
        constexpr auto Determinant() const;

        template <usize N = RowCount>
            requires(RowCount == ColCount && N == RowCount)
        constexpr Mat Inverse() const;
    };

    template <Numeric T, Numeric U, usize RowCount, usize ColCount, typename Layout>
    using MatCommon = Mat<std::common_type_t<T, U>, RowCount, ColCount, Layout>;

    template <Numeric T, Numeric U, usize RowCount, usize ColCount, typename Layout>
    constexpr auto operator+(const Mat<T, RowCount, ColCount, Layout>& a, const Mat<U, RowCount, ColCount, Layout>& b)
        -> MatCommon<T, U, RowCount, ColCount, Layout>;

    template <Numeric T, Numeric U, usize RowCount, usize ColCount, typename Layout>
    constexpr auto operator-(const Mat<T, RowCount, ColCount, Layout>& a, const Mat<U, RowCount, ColCount, Layout>& b)
        -> MatCommon<T, U, RowCount, ColCount, Layout>;

    template <Numeric T, Numeric U, usize RowCount, usize ColCount, typename Layout>
    constexpr auto operator*(const Mat<T, RowCount, ColCount, Layout>& mat, U scalar)
        -> MatCommon<T, U, RowCount, ColCount, Layout>;

    template <Numeric T, Numeric U, usize RowCount, usize ColCount, typename Layout>
    constexpr auto operator*(U scalar, const Mat<T, RowCount, ColCount, Layout>& mat)
        -> MatCommon<T, U, RowCount, ColCount, Layout>;

    template <Numeric T, Numeric U, usize RowCount, usize ColCount, typename Layout>
    constexpr auto operator/(const Mat<T, RowCount, ColCount, Layout>& mat, U scalar)
        -> MatCommon<T, U, RowCount, ColCount, Layout>;

    // (A x B) * (B x C) = (A x C)
    template <Numeric T, Numeric U, usize A, usize B, usize C, typename Layout>
    constexpr auto operator*(const Mat<T, A, B, Layout>& lhs, const Mat<U, B, C, Layout>& rhs)
        -> MatCommon<T, U, A, C, Layout>;

    template <Numeric T, usize RowCount, usize ColCount, typename FromLayout, typename ToLayout>
    constexpr Mat<T, RowCount, ColCount, ToLayout> MatCastLayout(const Mat<T, RowCount, ColCount, FromLayout>& mat);

    template <Numeric T, usize RowCount, usize ColCount>
    using MatRowMajor = Mat<T, RowCount, ColCount, RowMajor>;

    template <Numeric T, usize RowCount, usize ColCount>
    using MatColumnMajor = Mat<T, RowCount, ColCount, ColumnMajor>;

    template <Numeric T, usize N>
    using SquareMatrix = Mat<T, N, N>;

    template <Numeric T, usize N>
    using SquareMatrixRowMajor = Mat<T, N, N, RowMajor>;

    template <Numeric T, usize N>
    using SquareMatrixColumnMajor = Mat<T, N, N, ColumnMajor>;

    template <Numeric T>
    using Mat2 = Mat<T, 2, 2>;

    template <Numeric T>
    using Mat2ColumnMajor = Mat<T, 2, 2, ColumnMajor>;

    template <Numeric T>
    using Mat3 = Mat<T, 3, 3>;

    template <Numeric T>
    using Mat3ColumnMajor = Mat<T, 3, 3, ColumnMajor>;

    template <Numeric T>
    using Mat4 = Mat<T, 4, 4>;

    template <Numeric T>
    using Mat4ColumnMajor = Mat<T, 4, 4, ColumnMajor>;

    using Mat2f = Mat2<float32>;
    using Mat3f = Mat3<float32>;
    using Mat4f = Mat4<float32>;

    using Mat2fColumnMajor = Mat2ColumnMajor<float32>;
    using Mat3fColumnMajor = Mat3ColumnMajor<float32>;
    using Mat4fColumnMajor = Mat4ColumnMajor<float32>;

    using Mat2d = Mat2<float64>;
    using Mat3d = Mat3<float64>;
    using Mat4d = Mat4<float64>;

    using Mat2dColumnMajor = Mat2ColumnMajor<float64>;
    using Mat3dColumnMajor = Mat3ColumnMajor<float64>;
    using Mat4dColumnMajor = Mat4ColumnMajor<float64>;

    using Mat2i = Mat2<int>;
    using Mat3i = Mat3<int>;
    using Mat4i = Mat4<int>;

    using Mat2iColumnMajor = Mat2ColumnMajor<int>;
    using Mat3iColumnMajor = Mat3ColumnMajor<int>;
    using Mat4iColumnMajor = Mat4ColumnMajor<int>;

    using Mat2u = Mat2<unsigned int>;
    using Mat3u = Mat3<unsigned int>;
    using Mat4u = Mat4<unsigned int>;

    using Mat2uColumnMajor = Mat2ColumnMajor<unsigned int>;
    using Mat3uColumnMajor = Mat3ColumnMajor<unsigned int>;
    using Mat4uColumnMajor = Mat4ColumnMajor<unsigned int>;
} // namespace Nexus

template <Nexus::Numeric T, Nexus::usize RowCount, Nexus::usize ColCount, typename Layout>
struct std::formatter<Nexus::Mat<T, RowCount, ColCount, Layout>>;

#include "Mat.inl"
