// SPDX-License-Identifier: MIT

#pragma once

namespace Nexus {

    template <Numeric T, usize RowCount, usize ColCount, typename Layout>
    template <typename... Args>
        requires(sizeof...(Args) == RowCount * ColCount) && (std::convertible_to<Args, T> && ...)
    constexpr Mat<T, RowCount, ColCount, Layout>::Mat(Args&&... args) {
        const std::array<T, RowCount * ColCount> values{static_cast<T>(std::forward<Args>(args))...};

        for (size_type row = 0; row < RowCount; ++row) {
            for (size_type col = 0; col < ColCount; ++col) {
                (*this)[row, col] = values[row * ColCount + col];
            }
        }
    }

    template <Numeric T, usize RowCount, usize ColCount, typename Layout>
    template <Numeric U, typename OtherLayout>
    constexpr Mat<T, RowCount, ColCount, Layout>::Mat(const Mat<U, RowCount, ColCount, OtherLayout>& other) {
        for (size_type row = 0; row < RowCount; ++row) {
            for (size_type col = 0; col < ColCount; ++col) {
                (*this)[row, col] = static_cast<T>(other[row, col]);
            }
        }
    }

    template <Numeric T, usize RowCount, usize ColCount, typename Layout>
    constexpr T& Mat<T, RowCount, ColCount, Layout>::operator[](size_type row, size_type col) {
        return m_data[m_mapping(row, col)];
    }

    template <Numeric T, usize RowCount, usize ColCount, typename Layout>
    constexpr const T& Mat<T, RowCount, ColCount, Layout>::operator[](size_type row, size_type col) const {
        return m_data[m_mapping(row, col)];
    }

    template <Numeric T, usize RowCount, usize ColCount, typename Layout>
    constexpr T& Mat<T, RowCount, ColCount, Layout>::operator()(size_type row, size_type col) {
        return (*this)[row, col];
    }

    template <Numeric T, usize RowCount, usize ColCount, typename Layout>
    constexpr const T& Mat<T, RowCount, ColCount, Layout>::operator()(size_type row, size_type col) const {
        return (*this)[row, col];
    }

    template <Numeric T, usize RowCount, usize ColCount, typename Layout>
    constexpr T* Mat<T, RowCount, ColCount, Layout>::Data() {
        return m_data.data();
    }

    template <Numeric T, usize RowCount, usize ColCount, typename Layout>
    constexpr const T* Mat<T, RowCount, ColCount, Layout>::Data() const {
        return m_data.data();
    }

    template <Numeric T, usize RowCount, usize ColCount, typename Layout>
    constexpr auto Mat<T, RowCount, ColCount, Layout>::MDSpan() {
        return mdspan_type{m_data.data()};
    }

    template <Numeric T, usize RowCount, usize ColCount, typename Layout>
    constexpr auto Mat<T, RowCount, ColCount, Layout>::MDSpan() const {
        return const_mdspan_type{m_data.data()};
    }

    template <Numeric T, usize RowCount, usize ColCount, typename Layout>
    constexpr usize Mat<T, RowCount, ColCount, Layout>::Rows() {
        return RowCount;
    }

    template <Numeric T, usize RowCount, usize ColCount, typename Layout>
    constexpr usize Mat<T, RowCount, ColCount, Layout>::Cols() {
        return ColCount;
    }

    template <Numeric T, usize RowCount, usize ColCount, typename Layout>
    constexpr usize Mat<T, RowCount, ColCount, Layout>::Size() {
        return RowCount * ColCount;
    }

    template <Numeric T, usize RowCount, usize ColCount, typename Layout>
    template <Numeric U>
    constexpr bool
    Mat<T, RowCount, ColCount, Layout>::operator==(const Mat<U, RowCount, ColCount, Layout>& other) const {
        for (size_type row = 0; row < RowCount; ++row) {
            for (size_type col = 0; col < ColCount; ++col) {
                if ((*this)[row, col] != other[row, col]) {
                    return false;
                }
            }
        }

        return true;
    }

    template <Numeric T, usize RowCount, usize ColCount, typename Layout>
    template <Numeric U, typename OtherLayout>
    constexpr bool
    Mat<T, RowCount, ColCount, Layout>::operator==(const Mat<U, RowCount, ColCount, OtherLayout>& other) const {
        for (size_type row = 0; row < RowCount; ++row) {
            for (size_type col = 0; col < ColCount; ++col) {
                if ((*this)[row, col] != other[row, col]) {
                    return false;
                }
            }
        }

        return true;
    }

    template <Numeric T, usize RowCount, usize ColCount, typename Layout>
    template <Numeric U, typename OtherLayout>
    constexpr Mat<T, RowCount, ColCount, Layout>&
    Mat<T, RowCount, ColCount, Layout>::operator+=(const Mat<U, RowCount, ColCount, OtherLayout>& other) {
        for (size_type row = 0; row < RowCount; ++row) {
            for (size_type col = 0; col < ColCount; ++col) {
                (*this)[row, col] += other[row, col];
            }
        }

        return *this;
    }

    template <Numeric T, usize RowCount, usize ColCount, typename Layout>
    template <Numeric U, typename OtherLayout>
    constexpr Mat<T, RowCount, ColCount, Layout>&
    Mat<T, RowCount, ColCount, Layout>::operator-=(const Mat<U, RowCount, ColCount, OtherLayout>& other) {
        for (size_type row = 0; row < RowCount; ++row) {
            for (size_type col = 0; col < ColCount; ++col) {
                (*this)[row, col] -= other[row, col];
            }
        }

        return *this;
    }

    template <Numeric T, usize RowCount, usize ColCount, typename Layout>
    template <Numeric U>
    constexpr Mat<T, RowCount, ColCount, Layout>& Mat<T, RowCount, ColCount, Layout>::operator*=(U scalar) {
        for (auto& value : m_data) {
            value *= scalar;
        }

        return *this;
    }

    template <Numeric T, usize RowCount, usize ColCount, typename Layout>
    template <Numeric U>
    constexpr Mat<T, RowCount, ColCount, Layout>& Mat<T, RowCount, ColCount, Layout>::operator/=(U scalar) {
        for (auto& value : m_data) {
            value /= scalar;
        }

        return *this;
    }

    template <Numeric T, usize RowCount, usize ColCount, typename Layout>
    constexpr Mat<T, RowCount, ColCount, Layout> Mat<T, RowCount, ColCount, Layout>::operator+() const {
        return *this;
    }

    template <Numeric T, usize RowCount, usize ColCount, typename Layout>
    constexpr Mat<T, RowCount, ColCount, Layout> Mat<T, RowCount, ColCount, Layout>::operator-() const {
        Mat result;

        for (size_type row = 0; row < RowCount; ++row) {
            for (size_type col = 0; col < ColCount; ++col) {
                result[row, col] = -(*this)[row, col];
            }
        }

        return result;
    }

    template <Numeric T, usize RowCount, usize ColCount, typename Layout>
    constexpr Mat<T, ColCount, RowCount, Layout> Mat<T, RowCount, ColCount, Layout>::Transpose() const {
        Mat<T, ColCount, RowCount, Layout> result;

        for (size_type row = 0; row < RowCount; ++row) {
            for (size_type col = 0; col < ColCount; ++col) {
                result[col, row] = (*this)[row, col];
            }
        }

        return result;
    }

    template <Numeric T, usize RowCount, usize ColCount, typename Layout>
    template <usize N>
        requires(RowCount == ColCount && N == RowCount)
    constexpr Mat<T, RowCount, ColCount, Layout> Mat<T, RowCount, ColCount, Layout>::Identity() {
        Mat result;

        for (size_type i = 0; i < RowCount; ++i) {
            result[i, i] = T{1};
        }

        return result;
    }

    template <Numeric T, usize RowCount, usize ColCount, typename Layout>
    template <usize N>
        requires(RowCount == ColCount && N == RowCount)
    constexpr auto Mat<T, RowCount, ColCount, Layout>::Determinant() const {
        static_assert(RowCount == ColCount);

        Mat<T, RowCount, ColCount, Layout> temp = *this;
        T result = T{1};

        for (size_type i = 0; i < RowCount; ++i) {
            const T pivot = temp[i, i];

            if (pivot == T{}) {
                return T{};
            }

            result *= pivot;

            for (size_type row = i + 1; row < RowCount; ++row) {
                const T factor = temp[row, i] / pivot;

                for (size_type col = i; col < ColCount; ++col) {
                    temp[row, col] -= factor * temp[i, col];
                }
            }
        }

        return result;
    }

    template <Numeric T, usize RowCount, usize ColCount, typename Layout>
    template <usize N>
        requires(RowCount == ColCount && N == RowCount)
    constexpr Mat<T, RowCount, ColCount, Layout> Mat<T, RowCount, ColCount, Layout>::Inverse() const {
        static_assert(RowCount == ColCount);

        Mat<T, RowCount, ColCount * 2, Layout> augmented;

        for (size_type row = 0; row < RowCount; ++row) {
            for (size_type col = 0; col < ColCount; ++col) {
                augmented[row, col] = (*this)[row, col];
            }

            for (size_type col = 0; col < ColCount; ++col) {
                augmented[row, ColCount + col] = row == col ? T{1} : T{};
            }
        }

        for (size_type i = 0; i < RowCount; ++i) {
            const T pivot = augmented[i, i];

            if (pivot == T{}) {
                return Mat{};
            }

            for (size_type col = 0; col < ColCount * 2; ++col) {
                augmented[i, col] /= pivot;
            }

            for (size_type row = 0; row < RowCount; ++row) {
                if (row == i) {
                    continue;
                }

                const T factor = augmented[row, i];

                for (size_type col = 0; col < ColCount * 2; ++col) {
                    augmented[row, col] -= factor * augmented[i, col];
                }
            }
        }

        Mat result;

        for (size_type row = 0; row < RowCount; ++row) {
            for (size_type col = 0; col < ColCount; ++col) {
                result[row, col] = augmented[row, ColCount + col];
            }
        }

        return result;
    }

    template <Numeric T, Numeric U, usize RowCount, usize ColCount, typename Layout>
    constexpr auto operator+(const Mat<T, RowCount, ColCount, Layout>& a, const Mat<U, RowCount, ColCount, Layout>& b)
        -> MatCommon<T, U, RowCount, ColCount, Layout> {
        using R = std::common_type_t<T, U>;

        MatCommon<T, U, RowCount, ColCount, Layout> result;

        for (usize row = 0; row < RowCount; ++row) {
            for (usize col = 0; col < ColCount; ++col) {
                result[row, col] = static_cast<R>(a[row, col]) + static_cast<R>(b[row, col]);
            }
        }

        return result;
    }

    template <Numeric T, Numeric U, usize RowCount, usize ColCount, typename Layout>
    constexpr auto operator-(const Mat<T, RowCount, ColCount, Layout>& a, const Mat<U, RowCount, ColCount, Layout>& b)
        -> MatCommon<T, U, RowCount, ColCount, Layout> {
        using R = std::common_type_t<T, U>;

        MatCommon<T, U, RowCount, ColCount, Layout> result;

        for (usize row = 0; row < RowCount; ++row) {
            for (usize col = 0; col < ColCount; ++col) {
                result[row, col] = static_cast<R>(a[row, col]) - static_cast<R>(b[row, col]);
            }
        }

        return result;
    }

    template <Numeric T, Numeric U, usize RowCount, usize ColCount, typename Layout>
    constexpr auto operator*(const Mat<T, RowCount, ColCount, Layout>& mat, U scalar)
        -> MatCommon<T, U, RowCount, ColCount, Layout> {
        using R = std::common_type_t<T, U>;

        MatCommon<T, U, RowCount, ColCount, Layout> result;

        for (usize row = 0; row < RowCount; ++row) {
            for (usize col = 0; col < ColCount; ++col) {
                result[row, col] = static_cast<R>(mat[row, col]) * static_cast<R>(scalar);
            }
        }

        return result;
    }

    template <Numeric T, Numeric U, usize RowCount, usize ColCount, typename Layout>
    constexpr auto operator*(U scalar, const Mat<T, RowCount, ColCount, Layout>& mat)
        -> MatCommon<T, U, RowCount, ColCount, Layout> {
        return mat * scalar;
    }

    template <Numeric T, Numeric U, usize RowCount, usize ColCount, typename Layout>
    constexpr auto operator/(const Mat<T, RowCount, ColCount, Layout>& mat, U scalar)
        -> MatCommon<T, U, RowCount, ColCount, Layout> {
        using R = std::common_type_t<T, U>;

        MatCommon<T, U, RowCount, ColCount, Layout> result;

        for (usize row = 0; row < RowCount; ++row) {
            for (usize col = 0; col < ColCount; ++col) {
                result[row, col] = static_cast<R>(mat[row, col]) / static_cast<R>(scalar);
            }
        }

        return result;
    }

    template <Numeric T, Numeric U, usize A, usize B, usize C, typename Layout>
    constexpr auto operator*(const Mat<T, A, B, Layout>& lhs, const Mat<U, B, C, Layout>& rhs)
        -> MatCommon<T, U, A, C, Layout> {
        using R = std::common_type_t<T, U>;

        MatCommon<T, U, A, C, Layout> result;

        for (usize row = 0; row < A; ++row) {
            for (usize col = 0; col < C; ++col) {
                R value{};

                for (usize i = 0; i < B; ++i) {
                    value += static_cast<R>(lhs[row, i]) * static_cast<R>(rhs[i, col]);
                }

                result[row, col] = value;
            }
        }

        return result;
    }

    template <Numeric T, usize RowCount, usize ColCount, typename FromLayout, typename ToLayout>
    constexpr Mat<T, RowCount, ColCount, ToLayout> MatCastLayout(const Mat<T, RowCount, ColCount, FromLayout>& mat) {
        Mat<T, RowCount, ColCount, ToLayout> result;

        for (usize row = 0; row < RowCount; ++row) {
            for (usize col = 0; col < ColCount; ++col) {
                result[row, col] = mat[row, col];
            }
        }

        return result;
    }

} // namespace Nexus

template <Nexus::Numeric T, Nexus::usize RowCount, Nexus::usize ColCount, typename Layout>
struct std::formatter<Nexus::Mat<T, RowCount, ColCount, Layout>> {
    std::formatter<T> underlying;

    constexpr auto parse(std::format_parse_context& ctx) {
        return underlying.parse(ctx);
    }

    auto format(const Nexus::Mat<T, RowCount, ColCount, Layout>& obj, std::format_context& ctx) const {
        auto out = ctx.out();

        out = std::format_to(out, "[");

        for (Nexus::usize row = 0; row < RowCount; ++row) {
            if (row != 0) {
                out = std::format_to(out, ", ");
            }

            out = std::format_to(out, "[");

            for (Nexus::usize col = 0; col < ColCount; ++col) {
                if (col != 0) {
                    out = std::format_to(out, ", ");
                }

                out = underlying.format(obj[row, col], ctx);
            }

            out = std::format_to(out, "]");
        }

        return std::format_to(out, "]");
    }
};
