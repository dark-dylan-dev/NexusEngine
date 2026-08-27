// SPDX-License-Identifier: MIT

#pragma once

namespace Nexus {
    template <Numeric T>
    constexpr Vec3<T>::Vec3(T x, T y, T z) : x(x),
                                             y(y),
                                             z(z) {}

    template <Numeric T>
    template <Numeric U>
    constexpr Vec3<T>& Vec3<T>::operator+=(const Vec3<U>& other) {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }

    template <Numeric T>
    template <Numeric U>
    constexpr Vec3<T>& Vec3<T>::operator-=(const Vec3<U>& other) {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }

    template <Numeric T>
    template <Numeric U>
    constexpr Vec3<T>& Vec3<T>::operator*=(const Vec3<U>& other) {
        x *= other.x;
        y *= other.y;
        z *= other.z;
        return *this;
    }

    template <Numeric T>
    template <Numeric U>
    constexpr Vec3<T>& Vec3<T>::operator/=(const Vec3<U>& other) {
        x /= other.x;
        y /= other.y;
        z /= other.z;
        return *this;
    }

    template <Numeric T>
    template <Numeric U>
    constexpr Vec3<T>& Vec3<T>::operator*=(U scalar) {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }

    template <Numeric T>
    template <Numeric U>
    constexpr Vec3<T>& Vec3<T>::operator/=(U scalar) {
        x /= scalar;
        y /= scalar;
        z /= scalar;
        return *this;
    }

    template <Numeric T>
    constexpr Vec3<T> Vec3<T>::operator+() const {
        return *this;
    }

    template <Numeric T>
    constexpr Vec3<T> Vec3<T>::operator-() const {
        return {-x, -y, -z};
    }

    template <Numeric T>
    template <Numeric U>
    constexpr bool Vec3<T>::operator==(const Vec3<U>& other) const {
        return x == other.x && y == other.y && z == other.z;
    }

    template <Numeric T, Numeric U>
    constexpr auto operator+(const Vec3<T>& a, const Vec3<U>& b) -> Vec3Common<T, U> {
        using R = std::common_type_t<T, U>;
        return {static_cast<R>(a.x) + static_cast<R>(b.x), static_cast<R>(a.y) + static_cast<R>(b.y),
                static_cast<R>(a.z) + static_cast<R>(b.z)};
    }

    template <Numeric T, Numeric U>
    constexpr auto operator-(const Vec3<T>& a, const Vec3<U>& b) -> Vec3Common<T, U> {
        using R = std::common_type_t<T, U>;
        return {static_cast<R>(a.x) - static_cast<R>(b.x), static_cast<R>(a.y) - static_cast<R>(b.y),
                static_cast<R>(a.z) - static_cast<R>(b.z)};
    }

    template <Numeric T, Numeric U>
    constexpr auto operator*(const Vec3<T>& a, const Vec3<U>& b) -> Vec3Common<T, U> {
        using R = std::common_type_t<T, U>;
        return {static_cast<R>(a.x) * static_cast<R>(b.x), static_cast<R>(a.y) * static_cast<R>(b.y),
                static_cast<R>(a.z) * static_cast<R>(b.z)};
    }

    template <Numeric T, Numeric U>
    constexpr auto operator/(const Vec3<T>& a, const Vec3<U>& b) -> Vec3Common<T, U> {
        using R = std::common_type_t<T, U>;
        return {static_cast<R>(a.x) / static_cast<R>(b.x), static_cast<R>(a.y) / static_cast<R>(b.y),
                static_cast<R>(a.z) / static_cast<R>(b.z)};
    }

    template <Numeric T, Numeric U>
    constexpr auto operator*(const Vec3<T>& v, U scalar) -> Vec3Common<T, U> {
        using R = std::common_type_t<T, U>;
        return {static_cast<R>(v.x) * static_cast<R>(scalar), static_cast<R>(v.y) * static_cast<R>(scalar),
                static_cast<R>(v.z) * static_cast<R>(scalar)};
    }

    template <Numeric T, Numeric U>
    constexpr auto operator*(U scalar, const Vec3<T>& v) -> Vec3Common<T, U> {
        return v * scalar;
    }

    template <Numeric T, Numeric U>
    constexpr auto operator/(const Vec3<T>& v, U scalar) -> Vec3Common<T, U> {
        using R = std::common_type_t<T, U>;
        return {static_cast<R>(v.x) / static_cast<R>(scalar), static_cast<R>(v.y) / static_cast<R>(scalar),
                static_cast<R>(v.z) / static_cast<R>(scalar)};
    }
} // namespace Nexus

template <Nexus::Numeric T>
struct std::formatter<Nexus::Vec3<T>> {
    std::formatter<T> underlying;

    constexpr auto parse(std::format_parse_context& ctx) {
        return underlying.parse(ctx);
    }

    auto format(const Nexus::Vec3<T>& obj, std::format_context& ctx) const {
        auto out = ctx.out();
        out = std::format_to(out, "(");
        out = underlying.format(obj.x, ctx);
        out = std::format_to(out, ", ");
        out = underlying.format(obj.y, ctx);
        out = std::format_to(out, ", ");
        out = underlying.format(obj.z, ctx);
        return std::format_to(out, ")");
    }
};
