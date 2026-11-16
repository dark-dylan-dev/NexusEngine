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

    template <Numeric T, usize N>
    constexpr T Vec<T, N>::LengthSquared() const {
        T result{};

        for (size_type i = 0; i < N; ++i) {
            result += (*this)[i] * (*this)[i];
        }

        return result;
    }

    template <Numeric T, usize N>
    constexpr T Vec<T, N>::Length() const {
        return static_cast<T>(std::sqrt(static_cast<float64>(LengthSquared())));
    }

    template <Numeric T, usize N>
    constexpr Vec<T, N> Vec<T, N>::Normalized() const {
        const T length = Length();

        if (length <= T{0}) {
            return *this;
        }

        return *this * (T{1} / length);
    }

    template <Numeric T, usize N>
    constexpr void Vec<T, N>::Normalize() {
        *this = Normalized();
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

    template <Numeric T, Numeric U, usize N>
    constexpr auto Dot(const Vec<T, N>& a, const Vec<U, N>& b) -> std::common_type_t<T, U> {
        using R = std::common_type_t<T, U>;

        R result{};

        for (usize i = 0; i < N; ++i) {
            result += static_cast<R>(a[i]) * static_cast<R>(b[i]);
        }

        return result;
    }

    template <Numeric T, Numeric U>
    constexpr auto Cross(const Vec<T, 3>& a, const Vec<U, 3>& b) -> VecCommon<T, U, 3> {
        using R = std::common_type_t<T, U>;

        return VecCommon<T, U, 3>{
            static_cast<R>(a[1]) * static_cast<R>(b[2]) - static_cast<R>(a[2]) * static_cast<R>(b[1]),
            static_cast<R>(a[2]) * static_cast<R>(b[0]) - static_cast<R>(a[0]) * static_cast<R>(b[2]),
            static_cast<R>(a[0]) * static_cast<R>(b[1]) - static_cast<R>(a[1]) * static_cast<R>(b[0])};
    }

    template <Numeric T, Numeric U, usize N>
    constexpr auto Distance(const Vec<T, N>& a, const Vec<U, N>& b) -> std::common_type_t<T, U> {
        return (a - b).Length();
    }

    template <Numeric T, Numeric U, Numeric V, usize N>
    constexpr auto Lerp(const Vec<T, N>& a, const Vec<U, N>& b, V t) -> VecCommon<T, U, N> {
        using R = std::common_type_t<T, U>;

        VecCommon<T, U, N> result;

        for (usize i = 0; i < N; ++i) {
            const R av = static_cast<R>(a[i]);
            const R bv = static_cast<R>(b[i]);
            result[i] = av + (bv - av) * static_cast<R>(t);
        }

        return result;
    }

    template <Numeric T, Numeric U, usize N>
    constexpr auto Scale(const Vec<T, N>& a, const Vec<U, N>& b) -> VecCommon<T, U, N> {
        return a * b;
    }

    template <Numeric T, Numeric U, usize N>
    constexpr auto Min(const Vec<T, N>& a, const Vec<U, N>& b) -> VecCommon<T, U, N> {
        using R = std::common_type_t<T, U>;

        VecCommon<T, U, N> result;

        for (usize i = 0; i < N; ++i) {
            const R av = static_cast<R>(a[i]);
            const R bv = static_cast<R>(b[i]);
            result[i] = av < bv ? av : bv;
        }

        return result;
    }

    template <Numeric T, Numeric U, usize N>
    constexpr auto Max(const Vec<T, N>& a, const Vec<U, N>& b) -> VecCommon<T, U, N> {
        using R = std::common_type_t<T, U>;

        VecCommon<T, U, N> result;

        for (usize i = 0; i < N; ++i) {
            const R av = static_cast<R>(a[i]);
            const R bv = static_cast<R>(b[i]);
            result[i] = av > bv ? av : bv;
        }

        return result;
    }

    template <Numeric T, Numeric U, Numeric V, usize N>
    constexpr auto MoveTowards(const Vec<T, N>& current, const Vec<U, N>& target, V maxDistanceDelta)
        -> VecCommon<T, U, N> {
        using R = std::common_type_t<T, U>;

        const VecCommon<T, U, N> delta = target - current;
        const R distance = delta.Length();

        if (distance <= static_cast<R>(maxDistanceDelta) || distance <= R{0}) {
            VecCommon<T, U, N> result;

            for (usize i = 0; i < N; ++i) {
                result[i] = static_cast<R>(target[i]);
            }

            return result;
        }

        VecCommon<T, U, N> result;

        for (usize i = 0; i < N; ++i) {
            result[i] = static_cast<R>(current[i]) + delta[i] * (static_cast<R>(maxDistanceDelta) / distance);
        }

        return result;
    }

    template <Numeric T, Numeric U, usize N>
    constexpr auto Reflect(const Vec<T, N>& v, const Vec<U, N>& normal) -> VecCommon<T, U, N> {
        using R = std::common_type_t<T, U>;

        VecCommon<T, U, N> result;
        const R factor = R{2} * Dot(v, normal);

        for (usize i = 0; i < N; ++i) {
            result[i] = static_cast<R>(v[i]) - static_cast<R>(normal[i]) * factor;
        }

        return result;
    }

    template <Numeric T, Numeric U, usize N>
    constexpr auto Angle(const Vec<T, N>& a, const Vec<U, N>& b) -> std::common_type_t<T, U> {
        using R = std::common_type_t<T, U>;

        R lengthSquaredA{};
        R lengthSquaredB{};

        for (usize i = 0; i < N; ++i) {
            lengthSquaredA += static_cast<R>(a[i]) * static_cast<R>(a[i]);
            lengthSquaredB += static_cast<R>(b[i]) * static_cast<R>(b[i]);
        }

        const R denom = static_cast<R>(std::sqrt(static_cast<float64>(lengthSquaredA))) *
                        static_cast<R>(std::sqrt(static_cast<float64>(lengthSquaredB)));

        if (denom <= R{0}) {
            return R{0};
        }

        R cosAngle = Dot(a, b) / denom;
        cosAngle = cosAngle < R{-1} ? R{-1} : (cosAngle > R{1} ? R{1} : cosAngle);

        return static_cast<R>(std::acos(static_cast<float64>(cosAngle)));
    }

    template <Numeric T>
    constexpr Vec<T, 2> Perpendicular(const Vec<T, 2>& v) {
        return Vec<T, 2>{-v[1], v[0]};
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
