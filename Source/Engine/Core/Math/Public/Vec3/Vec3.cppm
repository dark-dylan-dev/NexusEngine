// SPDX-License-Identifier: MIT

module;

#include <Platform/DLL/Export.hpp>

export module NE.Engine.Math.Vec3;

import NE.Engine.Core.Config;
import NE.Engine.Core.Types;

import std;

export namespace Nexus {
    template <Numeric T>
    class NEXUS_API Vec3 {
    public:
        using value_type = T;

        T x{};
        T y{};
        T z{};

        constexpr Vec3() = default;
        constexpr Vec3(T x, T y, T z);

        template <Numeric U>
        constexpr bool operator==(const Vec3<U>& other) const;

        template <Numeric U>
        constexpr Vec3& operator+=(const Vec3<U>& other);

        template <Numeric U>
        constexpr Vec3& operator-=(const Vec3<U>& other);

        template <Numeric U>
        constexpr Vec3& operator*=(const Vec3<U>& other);

        template <Numeric U>
        constexpr Vec3& operator/=(const Vec3<U>& other);

        template <Numeric U>
        constexpr Vec3& operator*=(U scalar);

        template <Numeric U>
        constexpr Vec3& operator/=(U scalar);

        constexpr Vec3 operator+() const;
        constexpr Vec3 operator-() const;
    };

    template <Numeric T, Numeric U>
    using Vec3Common = Vec3<std::common_type_t<T, U>>;

    template <Numeric T, Numeric U>
    constexpr auto operator+(const Vec3<T>& a, const Vec3<U>& b) -> Vec3Common<T, U>;

    template <Numeric T, Numeric U>
    constexpr auto operator-(const Vec3<T>& a, const Vec3<U>& b) -> Vec3Common<T, U>;

    template <Numeric T, Numeric U>
    constexpr auto operator*(const Vec3<T>& a, const Vec3<U>& b) -> Vec3Common<T, U>;

    template <Numeric T, Numeric U>
    constexpr auto operator/(const Vec3<T>& a, const Vec3<U>& b) -> Vec3Common<T, U>;

    template <Numeric T, Numeric U>
    constexpr auto operator*(const Vec3<T>& v, U scalar) -> Vec3Common<T, U>;

    template <Numeric T, Numeric U>
    constexpr auto operator*(U scalar, const Vec3<T>& v) -> Vec3Common<T, U>;

    template <Numeric T, Numeric U>
    constexpr auto operator/(const Vec3<T>& v, U scalar) -> Vec3Common<T, U>;
} // namespace Nexus

template <Nexus::Numeric T>
struct std::formatter<Nexus::Vec3<T>>;

#include "Vec3.inl"
