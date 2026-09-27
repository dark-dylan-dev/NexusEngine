// SPDX-License-Identifier: MIT

module;

#include <Platform/DLL/Export.hpp>

export module NE.Engine.Math.Vec;

import NE.Engine.Core.Config;
import NE.Engine.Core.Types;

import std;

export namespace Nexus {

    template <Numeric T, usize N>
    class NEXUS_API Vec {
    public:
        using value_type = T;
        using size_type = usize;

    private:
        std::array<T, N> m_data{};

    public:
        constexpr Vec() = default;

        template <typename... Args>
            requires(sizeof...(Args) == N) && (std::convertible_to<Args, T> && ...)
        constexpr Vec(Args&&... args);

        constexpr T& operator[](size_type index);
        constexpr const T& operator[](size_type index) const;

        constexpr T* Data();
        constexpr const T* Data() const;

        static constexpr size_type Size();

        template <Numeric U>
        constexpr bool operator==(const Vec<U, N>& other) const;

        template <Numeric U>
        constexpr Vec& operator+=(const Vec<U, N>& other);

        template <Numeric U>
        constexpr Vec& operator-=(const Vec<U, N>& other);

        template <Numeric U>
        constexpr Vec& operator*=(const Vec<U, N>& other);

        template <Numeric U>
        constexpr Vec& operator/=(const Vec<U, N>& other);

        template <Numeric U>
        constexpr Vec& operator*=(U scalar);

        template <Numeric U>
        constexpr Vec& operator/=(U scalar);

        constexpr Vec operator+() const;
        constexpr Vec operator-() const;

        constexpr T LengthSquared() const;
        constexpr T Length() const;

        constexpr Vec Normalized() const;
        constexpr void Normalize();
    };

    template <Numeric T, Numeric U, usize N>
    using VecCommon = Vec<std::common_type_t<T, U>, N>;

    template <Numeric T, Numeric U, usize N>
    constexpr auto operator+(const Vec<T, N>& a, const Vec<U, N>& b) -> VecCommon<T, U, N>;

    template <Numeric T, Numeric U, usize N>
    constexpr auto operator-(const Vec<T, N>& a, const Vec<U, N>& b) -> VecCommon<T, U, N>;

    template <Numeric T, Numeric U, usize N>
    constexpr auto operator*(const Vec<T, N>& a, const Vec<U, N>& b) -> VecCommon<T, U, N>;

    template <Numeric T, Numeric U, usize N>
    constexpr auto operator/(const Vec<T, N>& a, const Vec<U, N>& b) -> VecCommon<T, U, N>;

    template <Numeric T, Numeric U, usize N>
    constexpr auto operator*(const Vec<T, N>& v, U scalar) -> VecCommon<T, U, N>;

    template <Numeric T, Numeric U, usize N>
    constexpr auto operator*(U scalar, const Vec<T, N>& v) -> VecCommon<T, U, N>;

    template <Numeric T, Numeric U, usize N>
    constexpr auto operator/(const Vec<T, N>& v, U scalar) -> VecCommon<T, U, N>;

    template <Numeric T, Numeric U, usize N>
    constexpr auto Dot(const Vec<T, N>& a, const Vec<U, N>& b) -> std::common_type_t<T, U>;

    template <Numeric T, Numeric U>
    constexpr auto Cross(const Vec<T, 3>& a, const Vec<U, 3>& b) -> VecCommon<T, U, 3>;

    template <Numeric T, Numeric U, usize N>
    constexpr auto Distance(const Vec<T, N>& a, const Vec<U, N>& b) -> std::common_type_t<T, U>;

    template <Numeric T, Numeric U, Numeric V, usize N>
    constexpr auto Lerp(const Vec<T, N>& a, const Vec<U, N>& b, V t) -> VecCommon<T, U, N>;

    template <Numeric T, Numeric U, usize N>
    constexpr auto Scale(const Vec<T, N>& a, const Vec<U, N>& b) -> VecCommon<T, U, N>;

    template <Numeric T, Numeric U, usize N>
    constexpr auto Min(const Vec<T, N>& a, const Vec<U, N>& b) -> VecCommon<T, U, N>;

    template <Numeric T, Numeric U, usize N>
    constexpr auto Max(const Vec<T, N>& a, const Vec<U, N>& b) -> VecCommon<T, U, N>;

    template <Numeric T, Numeric U, Numeric V, usize N>
    constexpr auto MoveTowards(const Vec<T, N>& current, const Vec<U, N>& target, V maxDistanceDelta)
        -> VecCommon<T, U, N>;

    template <Numeric T, Numeric U, usize N>
    constexpr auto Reflect(const Vec<T, N>& v, const Vec<U, N>& normal) -> VecCommon<T, U, N>;

    template <Numeric T, Numeric U, usize N>
    constexpr auto Angle(const Vec<T, N>& a, const Vec<U, N>& b) -> std::common_type_t<T, U>;

    template <Numeric T>
    constexpr Vec<T, 2> Perpendicular(const Vec<T, 2>& v);

    template <Numeric T>
    using Vec2 = Vec<T, 2>;

    template <Numeric T>
    using Vec3 = Vec<T, 3>;

    template <Numeric T>
    using Vec4 = Vec<T, 4>;

    using Vec2f = Vec2<float32>;
    using Vec3f = Vec3<float32>;
    using Vec4f = Vec4<float32>;

    using Vec2d = Vec2<float64>;
    using Vec3d = Vec3<float64>;
    using Vec4d = Vec4<float64>;

    using Vec2i = Vec2<int>;
    using Vec3i = Vec3<int>;
    using Vec4i = Vec4<int>;

    using Vec2u = Vec2<unsigned int>;
    using Vec3u = Vec3<unsigned int>;
    using Vec4u = Vec4<unsigned int>;
} // namespace Nexus

template <Nexus::Numeric T, Nexus::usize N>
struct std::formatter<Nexus::Vec<T, N>>;

#include "Vec.inl"
