// SPDX-License-Identifier: MIT

#pragma once

namespace Nexus {

    template <Numeric T, usize N>
    template <typename... Args>
        requires(sizeof...(Args) == N) && (std::convertible_to<Args, T> && ...)
    constexpr Vec<T, N>::Vec(Args&&... args) : m_data{static_cast<T>(std::forward<Args>(args))...} {}

    template <Numeric T, usize N>
    constexpr T& Vec<T, N>::operator[](size_type index) {
        return m_data[index];
    }

    template <Numeric T, usize N>
    constexpr const T& Vec<T, N>::operator[](size_type index) const {
        return m_data[index];
    }

    template <Numeric T, usize N>
    constexpr T* Vec<T, N>::Data() {
        return m_data.data();
    }

    template <Numeric T, usize N>
    constexpr const T* Vec<T, N>::Data() const {
        return m_data.data();
    }

    template <Numeric T, usize N>
    constexpr usize Vec<T, N>::Size() {
        return N;
    }

    template <Numeric T, usize N>
    template <Numeric U>
    constexpr bool Vec<T, N>::operator==(const Vec<U, N>& other) const {
        for (size_type i = 0; i < N; ++i) {
            if ((*this)[i] != other[i]) {
                return false;
            }
        }

        return true;
    }

    template <Numeric T, usize N>
    template <Numeric U>
    constexpr Vec<T, N>& Vec<T, N>::operator+=(const Vec<U, N>& other) {
        for (size_type i = 0; i < N; ++i) {
            (*this)[i] += other[i];
        }

        return *this;
    }

    template <Numeric T, usize N>
    template <Numeric U>
    constexpr Vec<T, N>& Vec<T, N>::operator-=(const Vec<U, N>& other) {
        for (size_type i = 0; i < N; ++i) {
            (*this)[i] -= other[i];
        }

        return *this;
    }

    template <Numeric T, usize N>
    template <Numeric U>
    constexpr Vec<T, N>& Vec<T, N>::operator*=(const Vec<U, N>& other) {
        for (size_type i = 0; i < N; ++i) {
            (*this)[i] *= other[i];
        }

        return *this;
    }

    template <Numeric T, usize N>
    template <Numeric U>
    constexpr Vec<T, N>& Vec<T, N>::operator/=(const Vec<U, N>& other) {
        for (size_type i = 0; i < N; ++i) {
            (*this)[i] /= other[i];
        }

        return *this;
    }

    template <Numeric T, usize N>
    template <Numeric U>
    constexpr Vec<T, N>& Vec<T, N>::operator*=(U scalar) {
        for (auto& value : m_data) {
            value *= scalar;
        }

        return *this;
    }

    template <Numeric T, usize N>
    template <Numeric U>
    constexpr Vec<T, N>& Vec<T, N>::operator/=(U scalar) {
        for (auto& value : m_data) {
            value /= scalar;
        }

        return *this;
    }

    template <Numeric T, usize N>
    constexpr Vec<T, N> Vec<T, N>::operator+() const {
        return *this;
    }

    template <Numeric T, usize N>
    constexpr Vec<T, N> Vec<T, N>::operator-() const {
        Vec result;

        for (size_type i = 0; i < N; ++i) {
            result[i] = -(*this)[i];
        }

        return result;
    }

    template <Numeric T, Numeric U, usize N>
    constexpr auto operator+(const Vec<T, N>& a, const Vec<U, N>& b) -> VecCommon<T, U, N> {
        using R = std::common_type_t<T, U>;

        VecCommon<T, U, N> result;

        for (usize i = 0; i < N; ++i) {
            result[i] = static_cast<R>(a[i]) + static_cast<R>(b[i]);
        }

        return result;
    }

    template <Numeric T, Numeric U, usize N>
    constexpr auto operator-(const Vec<T, N>& a, const Vec<U, N>& b) -> VecCommon<T, U, N> {
        using R = std::common_type_t<T, U>;

        VecCommon<T, U, N> result;

        for (usize i = 0; i < N; ++i) {
            result[i] = static_cast<R>(a[i]) - static_cast<R>(b[i]);
        }

        return result;
    }

    template <Numeric T, Numeric U, usize N>
    constexpr auto operator*(const Vec<T, N>& a, const Vec<U, N>& b) -> VecCommon<T, U, N> {
        using R = std::common_type_t<T, U>;

        VecCommon<T, U, N> result;

        for (usize i = 0; i < N; ++i) {
            result[i] = static_cast<R>(a[i]) * static_cast<R>(b[i]);
        }

        return result;
    }

    template <Numeric T, Numeric U, usize N>
    constexpr auto operator/(const Vec<T, N>& a, const Vec<U, N>& b) -> VecCommon<T, U, N> {
        using R = std::common_type_t<T, U>;

        VecCommon<T, U, N> result;

        for (usize i = 0; i < N; ++i) {
            result[i] = static_cast<R>(a[i]) / static_cast<R>(b[i]);
        }

        return result;
    }

    template <Numeric T, Numeric U, usize N>
    constexpr auto operator*(const Vec<T, N>& v, U scalar) -> VecCommon<T, U, N> {
        using R = std::common_type_t<T, U>;

        VecCommon<T, U, N> result;

        for (usize i = 0; i < N; ++i) {
            result[i] = static_cast<R>(v[i]) * static_cast<R>(scalar);
        }

        return result;
    }

    template <Numeric T, Numeric U, usize N>
    constexpr auto operator*(U scalar, const Vec<T, N>& v) -> VecCommon<T, U, N> {
        return v * scalar;
    }

    template <Numeric T, Numeric U, usize N>
    constexpr auto operator/(const Vec<T, N>& v, U scalar) -> VecCommon<T, U, N> {
        using R = std::common_type_t<T, U>;

        VecCommon<T, U, N> result;

        for (usize i = 0; i < N; ++i) {
            result[i] = static_cast<R>(v[i]) / static_cast<R>(scalar);
        }

        return result;
    }

} // namespace Nexus

template <Nexus::Numeric T, Nexus::usize N>
struct std::formatter<Nexus::Vec<T, N>> {
    std::formatter<T> underlying;

    constexpr auto parse(std::format_parse_context& ctx) {
        return underlying.parse(ctx);
    }

    auto format(const Nexus::Vec<T, N>& obj, std::format_context& ctx) const {
        auto out = ctx.out();
        out = std::format_to(out, "(");

        for (Nexus::usize i = 0; i < N; ++i) {
            if (i != 0) {
                out = std::format_to(out, ", ");
            }
            out = underlying.format(obj[i], ctx);
        }

        return std::format_to(out, ")");
    }
};
