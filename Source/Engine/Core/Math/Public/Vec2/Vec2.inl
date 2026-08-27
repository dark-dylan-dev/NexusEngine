// SPDX-License-Identifier: MIT

#pragma once

namespace Nexus {
    template <Numeric T>
    constexpr Vec2<T>::Vec2(T x, T y) : x(x),
                                        y(y) {}

    template <Numeric T>
    template <Numeric U>
    constexpr Vec2<T>& Vec2<T>::operator+=(const Vec2<U>& other) {
        x += other.x;
        y += other.y;
        return *this;
    }

    template <Numeric T>
    template <Numeric U>
    constexpr Vec2<T>& Vec2<T>::operator-=(const Vec2<U>& other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    template <Numeric T>
    template <Numeric U>
    constexpr Vec2<T>& Vec2<T>::operator*=(const Vec2<U>& other) {
        x *= other.x;
        y *= other.y;
        return *this;
    }

    template <Numeric T>
    template <Numeric U>
    constexpr Vec2<T>& Vec2<T>::operator/=(const Vec2<U>& other) {
        x /= other.x;
        y /= other.y;
        return *this;
    }

    template <Numeric T>
    template <Numeric U>
    constexpr Vec2<T>& Vec2<T>::operator*=(U scalar) {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    template <Numeric T>
    template <Numeric U>
    constexpr Vec2<T>& Vec2<T>::operator/=(U scalar) {
        x /= scalar;
        y /= scalar;
        return *this;
    }

    template <Numeric T>
    constexpr Vec2<T> Vec2<T>::operator+() const {
        return *this;
    }

    template <Numeric T>
    constexpr Vec2<T> Vec2<T>::operator-() const {
        return {-x, -y};
    }

    template <Numeric T>
    template <Numeric U>
    constexpr bool Vec2<T>::operator==(const Vec2<U>& other) const {
        return x == other.x && y == other.y;
    }

    template <Numeric T, Numeric U>
    constexpr auto operator+(const Vec2<T>& a, const Vec2<U>& b) -> Vec2Common<T, U> {
        using R = std::common_type_t<T, U>;
        return {static_cast<R>(a.x) + static_cast<R>(b.x), static_cast<R>(a.y) + static_cast<R>(b.y)};
    }

    template <Numeric T, Numeric U>
    constexpr auto operator-(const Vec2<T>& a, const Vec2<U>& b) -> Vec2Common<T, U> {
        using R = std::common_type_t<T, U>;
        return {static_cast<R>(a.x) - static_cast<R>(b.x), static_cast<R>(a.y) - static_cast<R>(b.y)};
    }

    template <Numeric T, Numeric U>
    constexpr auto operator*(const Vec2<T>& a, const Vec2<U>& b) -> Vec2Common<T, U> {
        using R = std::common_type_t<T, U>;
        return {static_cast<R>(a.x) * static_cast<R>(b.x), static_cast<R>(a.y) * static_cast<R>(b.y)};
    }

    template <Numeric T, Numeric U>
    constexpr auto operator/(const Vec2<T>& a, const Vec2<U>& b) -> Vec2Common<T, U> {
        using R = std::common_type_t<T, U>;
        return {static_cast<R>(a.x) / static_cast<R>(b.x), static_cast<R>(a.y) / static_cast<R>(b.y)};
    }

    template <Numeric T, Numeric U>
    constexpr auto operator*(const Vec2<T>& v, U scalar) -> Vec2Common<T, U> {
        using R = std::common_type_t<T, U>;
        return {static_cast<R>(v.x) * static_cast<R>(scalar), static_cast<R>(v.y) * static_cast<R>(scalar)};
    }

    template <Numeric T, Numeric U>
    constexpr auto operator*(U scalar, const Vec2<T>& v) -> Vec2Common<T, U> {
        return v * scalar;
    }

    template <Numeric T, Numeric U>
    constexpr auto operator/(const Vec2<T>& v, U scalar) -> Vec2Common<T, U> {
        using R = std::common_type_t<T, U>;
        return {static_cast<R>(v.x) / static_cast<R>(scalar), static_cast<R>(v.y) / static_cast<R>(scalar)};
    }
} // namespace Nexus

template <Nexus::Numeric T>
struct std::formatter<Nexus::Vec2<T>> {
    std::formatter<T> underlying;

    constexpr auto parse(std::format_parse_context& ctx) {
        return underlying.parse(ctx);
    }

    auto format(const Nexus::Vec2<T>& obj, std::format_context& ctx) const {
        auto out = ctx.out();
        out = std::format_to(out, "(");
        out = underlying.format(obj.x, ctx);
        out = std::format_to(out, ", ");
        out = underlying.format(obj.y, ctx);
        return std::format_to(out, ")");
    }
};
