// SPDX-License-Identifier: MIT

module;

#include <Platform/DLL/Export.hpp>

export module NE.Engine.Math.Quaternion;

import NE.Engine.Core.Config;
import NE.Engine.Core.Types;
import NE.Engine.Math.Vec;

import std;

export namespace Nexus {

    template <std::floating_point T>
    class NEXUS_API Quaternion {
    public:
        using value_type = T;
        using size_type = usize;

    private:
        T m_x{};
        T m_y{};
        T m_z{};
        T m_w{1};

    public:
        constexpr Quaternion() = default;

        constexpr Quaternion(T x, T y, T z, T w);

        template <std::floating_point U>
        constexpr Quaternion(const Vec3<U>& axis, U angleRadians);

        static constexpr Quaternion Identity();

        template <std::floating_point U>
        static constexpr Quaternion FromEuler(U pitch, U yaw, U roll);

        template <std::floating_point U>
        static constexpr Quaternion FromAxisAngle(const Vec3<U>& axis, U angleRadians);

        constexpr T& X();
        constexpr const T& X() const;

        constexpr T& Y();
        constexpr const T& Y() const;

        constexpr T& Z();
        constexpr const T& Z() const;

        constexpr T& W();
        constexpr const T& W() const;

        constexpr T* Data();
        constexpr const T* Data() const;

        static constexpr size_type Size();

        template <std::floating_point U>
        constexpr bool operator==(const Quaternion<U>& other) const;

        template <std::floating_point U>
        constexpr Quaternion& operator+=(const Quaternion<U>& other);

        template <std::floating_point U>
        constexpr Quaternion& operator-=(const Quaternion<U>& other);

        template <std::floating_point U>
        constexpr Quaternion& operator*=(const Quaternion<U>& other);

        template <std::floating_point U>
        constexpr Quaternion& operator*=(U scalar);

        template <std::floating_point U>
        constexpr Quaternion& operator/=(U scalar);

        constexpr Quaternion operator+() const;
        constexpr Quaternion operator-() const;

        constexpr T LengthSquared() const;
        constexpr T Length() const;

        constexpr Quaternion Normalized() const;
        constexpr void Normalize();

        constexpr Quaternion Conjugate() const;
        constexpr Quaternion Inverse() const;

        constexpr Vec3<T> RotateVector(const Vec3<T>& v) const;

        constexpr Vec3<T> ToEuler() const;

        constexpr T Pitch() const;
        constexpr T Yaw() const;
        constexpr T Roll() const;
    };

    template <std::floating_point T, std::floating_point U>
    using QuaternionCommon = Quaternion<std::common_type_t<T, U>>;

    template <std::floating_point T, std::floating_point U>
    constexpr auto operator+(const Quaternion<T>& a, const Quaternion<U>& b) -> QuaternionCommon<T, U>;

    template <std::floating_point T, std::floating_point U>
    constexpr auto operator-(const Quaternion<T>& a, const Quaternion<U>& b) -> QuaternionCommon<T, U>;

    template <std::floating_point T, std::floating_point U>
    constexpr auto operator*(const Quaternion<T>& a, const Quaternion<U>& b) -> QuaternionCommon<T, U>;

    template <std::floating_point T, std::floating_point U>
    constexpr auto operator*(const Quaternion<T>& q, U scalar) -> QuaternionCommon<T, U>;

    template <std::floating_point T, std::floating_point U>
    constexpr auto operator*(U scalar, const Quaternion<T>& q) -> QuaternionCommon<T, U>;

    template <std::floating_point T, std::floating_point U>
    constexpr auto operator/(const Quaternion<T>& q, U scalar) -> QuaternionCommon<T, U>;

    template <std::floating_point T, std::floating_point U>
    constexpr auto operator*(const Quaternion<T>& q, const Vec3<U>& v) -> Vec3<std::common_type_t<T, U>>;

    template <std::floating_point T>
    constexpr T Dot(const Quaternion<T>& a, const Quaternion<T>& b);

    template <std::floating_point T>
    constexpr Quaternion<T> Lerp(const Quaternion<T>& a, const Quaternion<T>& b, T t);

    template <std::floating_point T>
    constexpr Quaternion<T> Slerp(const Quaternion<T>& a, const Quaternion<T>& b, T t);

    using Quaternionf = Quaternion<float32>;
    using Quaterniond = Quaternion<float64>;
} // namespace Nexus

template <std::floating_point T>
struct std::formatter<Nexus::Quaternion<T>>;

#include "Quaternion.inl"
