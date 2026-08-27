// SPDX-License-Identifier: MIT

module;

#include <Platform/DLL/Export.hpp>

export module NE.Engine.Math.Vec2;

import NE.Engine.Core.Config;
import NE.Engine.Core.Types;

import std;

export namespace Nexus {
    template <Numeric T>
    class NEXUS_API Vec2 {
    public:
        using value_type = T;

        T x{};
        T y{};

        constexpr Vec2() = default;
        constexpr Vec2(T x, T y);

        template <Numeric U>
        constexpr bool operator==(const Vec2<U>& other) const;

        template <Numeric U>
        constexpr Vec2& operator+=(const Vec2<U>& other);

        template <Numeric U>
        constexpr Vec2& operator-=(const Vec2<U>& other);

        template <Numeric U>
        constexpr Vec2& operator*=(const Vec2<U>& other);

        template <Numeric U>
        constexpr Vec2& operator/=(const Vec2<U>& other);

        template <Numeric U>
        constexpr Vec2& operator*=(U scalar);

        template <Numeric U>
        constexpr Vec2& operator/=(U scalar);

        constexpr Vec2 operator+() const;
        constexpr Vec2 operator-() const;
    };

    template <Numeric T, Numeric U>
    using Vec2Common = Vec2<std::common_type_t<T, U>>;

    template <Numeric T, Numeric U>
    constexpr auto operator+(const Vec2<T>& a, const Vec2<U>& b) -> Vec2Common<T, U>;

    template <Numeric T, Numeric U>
    constexpr auto operator-(const Vec2<T>& a, const Vec2<U>& b) -> Vec2Common<T, U>;

    template <Numeric T, Numeric U>
    constexpr auto operator*(const Vec2<T>& a, const Vec2<U>& b) -> Vec2Common<T, U>;

    template <Numeric T, Numeric U>
    constexpr auto operator/(const Vec2<T>& a, const Vec2<U>& b) -> Vec2Common<T, U>;

    template <Numeric T, Numeric U>
    constexpr auto operator*(const Vec2<T>& v, U scalar) -> Vec2Common<T, U>;

    template <Numeric T, Numeric U>
    constexpr auto operator*(U scalar, const Vec2<T>& v) -> Vec2Common<T, U>;

    template <Numeric T, Numeric U>
    constexpr auto operator/(const Vec2<T>& v, U scalar) -> Vec2Common<T, U>;
} // namespace Nexus

template <Nexus::Numeric T>
struct std::formatter<Nexus::Vec2<T>>;

#include "Vec2.inl"
