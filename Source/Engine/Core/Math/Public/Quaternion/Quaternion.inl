// SPDX-License-Identifier: MIT

#pragma once

namespace Nexus {

    template <std::floating_point T>
    constexpr Quaternion<T>::Quaternion(T x, T y, T z, T w) : m_x(x), m_y(y), m_z(z), m_w(w) {}

    template <std::floating_point T>
    template <std::floating_point U>
    constexpr Quaternion<T>::Quaternion(const Vec3<U>& axis, U angleRadians) {
        *this = FromAxisAngle(axis, angleRadians);
    }

    template <std::floating_point T>
    constexpr Quaternion<T> Quaternion<T>::Identity() {
        return Quaternion(T{0}, T{0}, T{0}, T{1});
    }

    template <std::floating_point T>
    template <std::floating_point U>
    constexpr Quaternion<T> Quaternion<T>::FromEuler(U pitch, U yaw, U roll) {
        const U halfPitch = pitch * U{0.5};
        const U halfYaw = yaw * U{0.5};
        const U halfRoll = roll * U{0.5};

        const U sp = std::sin(halfPitch);
        const U cp = std::cos(halfPitch);
        const U sy = std::sin(halfYaw);
        const U cy = std::cos(halfYaw);
        const U sr = std::sin(halfRoll);
        const U cr = std::cos(halfRoll);

        return Quaternion(
            static_cast<T>(sp * cy * cr - cp * sy * sr),
            static_cast<T>(cp * sy * cr + sp * cy * sr),
            static_cast<T>(cp * cy * sr - sp * sy * cr),
            static_cast<T>(cp * cy * cr + sp * sy * sr)
        );
    }

    template <std::floating_point T>
    template <std::floating_point U>
    constexpr Quaternion<T> Quaternion<T>::FromAxisAngle(const Vec3<U>& axis, U angleRadians) {
        const U halfAngle = angleRadians * U{0.5};
        const U s = std::sin(halfAngle);
        const U c = std::cos(halfAngle);

        Vec3<U> normalizedAxis = axis;
        const U lengthSquared = normalizedAxis[0] * normalizedAxis[0] + normalizedAxis[1] * normalizedAxis[1]
            + normalizedAxis[2] * normalizedAxis[2];

        if (lengthSquared > U{0}) {
            const U invLength = U{1} / std::sqrt(lengthSquared);
            normalizedAxis *= invLength;
        }

        return Quaternion(
            static_cast<T>(normalizedAxis[0] * s),
            static_cast<T>(normalizedAxis[1] * s),
            static_cast<T>(normalizedAxis[2] * s),
            static_cast<T>(c)
        );
    }

    template <std::floating_point T>
    constexpr T& Quaternion<T>::X() {
        return m_x;
    }

    template <std::floating_point T>
    constexpr const T& Quaternion<T>::X() const {
        return m_x;
    }

    template <std::floating_point T>
    constexpr T& Quaternion<T>::Y() {
        return m_y;
    }

    template <std::floating_point T>
    constexpr const T& Quaternion<T>::Y() const {
        return m_y;
    }

    template <std::floating_point T>
    constexpr T& Quaternion<T>::Z() {
        return m_z;
    }

    template <std::floating_point T>
    constexpr const T& Quaternion<T>::Z() const {
        return m_z;
    }

    template <std::floating_point T>
    constexpr T& Quaternion<T>::W() {
        return m_w;
    }

    template <std::floating_point T>
    constexpr const T& Quaternion<T>::W() const {
        return m_w;
    }

    template <std::floating_point T>
    constexpr T* Quaternion<T>::Data() {
        return &m_x;
    }

    template <std::floating_point T>
    constexpr const T* Quaternion<T>::Data() const {
        return &m_x;
    }

    template <std::floating_point T>
    constexpr usize Quaternion<T>::Size() {
        return 4;
    }

    template <std::floating_point T>
    template <std::floating_point U>
    constexpr bool Quaternion<T>::operator==(const Quaternion<U>& other) const {
        return m_x == other.X() && m_y == other.Y() && m_z == other.Z() && m_w == other.W();
    }

    template <std::floating_point T>
    template <std::floating_point U>
    constexpr Quaternion<T>& Quaternion<T>::operator+=(const Quaternion<U>& other) {
        m_x += static_cast<T>(other.X());
        m_y += static_cast<T>(other.Y());
        m_z += static_cast<T>(other.Z());
        m_w += static_cast<T>(other.W());

        return *this;
    }

    template <std::floating_point T>
    template <std::floating_point U>
    constexpr Quaternion<T>& Quaternion<T>::operator-=(const Quaternion<U>& other) {
        m_x -= static_cast<T>(other.X());
        m_y -= static_cast<T>(other.Y());
        m_z -= static_cast<T>(other.Z());
        m_w -= static_cast<T>(other.W());

        return *this;
    }

    template <std::floating_point T>
    template <std::floating_point U>
    constexpr Quaternion<T>& Quaternion<T>::operator*=(const Quaternion<U>& other) {
        const T x = m_w * static_cast<T>(other.X()) + m_x * static_cast<T>(other.W()) + m_y * static_cast<T>(other.Z())
            - m_z * static_cast<T>(other.Y());
        const T y = m_w * static_cast<T>(other.Y()) - m_x * static_cast<T>(other.Z()) + m_y * static_cast<T>(other.W())
            + m_z * static_cast<T>(other.X());
        const T z = m_w * static_cast<T>(other.Z()) + m_x * static_cast<T>(other.Y()) - m_y * static_cast<T>(other.X())
            + m_z * static_cast<T>(other.W());
        const T w = m_w * static_cast<T>(other.W()) - m_x * static_cast<T>(other.X()) - m_y * static_cast<T>(other.Y())
            - m_z * static_cast<T>(other.Z());

        m_x = x;
        m_y = y;
        m_z = z;
        m_w = w;

        return *this;
    }

    template <std::floating_point T>
    template <std::floating_point U>
    constexpr Quaternion<T>& Quaternion<T>::operator*=(U scalar) {
        m_x *= static_cast<T>(scalar);
        m_y *= static_cast<T>(scalar);
        m_z *= static_cast<T>(scalar);
        m_w *= static_cast<T>(scalar);

        return *this;
    }

    template <std::floating_point T>
    template <std::floating_point U>
    constexpr Quaternion<T>& Quaternion<T>::operator/=(U scalar) {
        m_x /= static_cast<T>(scalar);
        m_y /= static_cast<T>(scalar);
        m_z /= static_cast<T>(scalar);
        m_w /= static_cast<T>(scalar);

        return *this;
    }

    template <std::floating_point T>
    constexpr Quaternion<T> Quaternion<T>::operator+() const {
        return *this;
    }

    template <std::floating_point T>
    constexpr Quaternion<T> Quaternion<T>::operator-() const {
        return Quaternion(-m_x, -m_y, -m_z, -m_w);
    }

    template <std::floating_point T>
    constexpr T Quaternion<T>::LengthSquared() const {
        return m_x * m_x + m_y * m_y + m_z * m_z + m_w * m_w;
    }

    template <std::floating_point T>
    constexpr T Quaternion<T>::Length() const {
        return std::sqrt(LengthSquared());
    }

    template <std::floating_point T>
    constexpr Quaternion<T> Quaternion<T>::Normalized() const {
        const T length = Length();

        if (length <= T{0}) {
            return *this;
        }

        const T invLength = T{1} / length;
        return Quaternion(m_x * invLength, m_y * invLength, m_z * invLength, m_w * invLength);
    }

    template <std::floating_point T>
    constexpr void Quaternion<T>::Normalize() {
        *this = Normalized();
    }

    template <std::floating_point T>
    constexpr Quaternion<T> Quaternion<T>::Conjugate() const {
        return Quaternion(-m_x, -m_y, -m_z, m_w);
    }

    template <std::floating_point T>
    constexpr Quaternion<T> Quaternion<T>::Inverse() const {
        const T lengthSquared = LengthSquared();

        if (lengthSquared <= T{0}) {
            return Conjugate();
        }

        return Conjugate() / lengthSquared;
    }

    template <std::floating_point T>
    constexpr Vec3<T> Quaternion<T>::RotateVector(const Vec3<T>& v) const {
        const Vec3<T> q{m_x, m_y, m_z};

        const Vec3<T> cross1{
            q[1] * v[2] - q[2] * v[1],
            q[2] * v[0] - q[0] * v[2],
            q[0] * v[1] - q[1] * v[0]
        };

        const Vec3<T> cross2{
            q[1] * cross1[2] - q[2] * cross1[1],
            q[2] * cross1[0] - q[0] * cross1[2],
            q[0] * cross1[1] - q[1] * cross1[0]
        };

        return v + (cross1 * (T{2} * m_w)) + (cross2 * T{2});
    }

    template <std::floating_point T>
    constexpr Vec3<T> Quaternion<T>::ToEuler() const {
        return Vec3<T>{Pitch(), Yaw(), Roll()};
    }

    template <std::floating_point T>
    constexpr T Quaternion<T>::Pitch() const {
        const T sinPitch = T{2} * (m_w * m_x + m_y * m_z);
        const T cosPitch = T{1} - T{2} * (m_x * m_x + m_y * m_y);

        return std::atan2(sinPitch, cosPitch);
    }

    template <std::floating_point T>
    constexpr T Quaternion<T>::Yaw() const {
        const T sinYaw = T{2} * (m_w * m_y - m_z * m_x);

        if (std::abs(sinYaw) >= T{1}) {
            return std::copysign(std::numbers::pi_v<T> / T{2}, sinYaw);
        }

        return std::asin(sinYaw);
    }

    template <std::floating_point T>
    constexpr T Quaternion<T>::Roll() const {
        const T sinRoll = T{2} * (m_w * m_z + m_x * m_y);
        const T cosRoll = T{1} - T{2} * (m_y * m_y + m_z * m_z);

        return std::atan2(sinRoll, cosRoll);
    }

    template <std::floating_point T, std::floating_point U>
    constexpr auto operator+(const Quaternion<T>& a, const Quaternion<U>& b) -> QuaternionCommon<T, U> {
        using R = std::common_type_t<T, U>;

        return QuaternionCommon<T, U>(
            static_cast<R>(a.X()) + static_cast<R>(b.X()),
            static_cast<R>(a.Y()) + static_cast<R>(b.Y()),
            static_cast<R>(a.Z()) + static_cast<R>(b.Z()),
            static_cast<R>(a.W()) + static_cast<R>(b.W())
        );
    }

    template <std::floating_point T, std::floating_point U>
    constexpr auto operator-(const Quaternion<T>& a, const Quaternion<U>& b) -> QuaternionCommon<T, U> {
        using R = std::common_type_t<T, U>;

        return QuaternionCommon<T, U>(
            static_cast<R>(a.X()) - static_cast<R>(b.X()),
            static_cast<R>(a.Y()) - static_cast<R>(b.Y()),
            static_cast<R>(a.Z()) - static_cast<R>(b.Z()),
            static_cast<R>(a.W()) - static_cast<R>(b.W())
        );
    }

    template <std::floating_point T, std::floating_point U>
    constexpr auto operator*(const Quaternion<T>& a, const Quaternion<U>& b) -> QuaternionCommon<T, U> {
        using R = std::common_type_t<T, U>;

        QuaternionCommon<T, U> result(
            static_cast<R>(a.X()), static_cast<R>(a.Y()), static_cast<R>(a.Z()), static_cast<R>(a.W())
        );

        result *= QuaternionCommon<T, U>(
            static_cast<R>(b.X()), static_cast<R>(b.Y()), static_cast<R>(b.Z()), static_cast<R>(b.W())
        );

        return result;
    }

    template <std::floating_point T, std::floating_point U>
    constexpr auto operator*(const Quaternion<T>& q, U scalar) -> QuaternionCommon<T, U> {
        using R = std::common_type_t<T, U>;

        return QuaternionCommon<T, U>(
            static_cast<R>(q.X()) * static_cast<R>(scalar),
            static_cast<R>(q.Y()) * static_cast<R>(scalar),
            static_cast<R>(q.Z()) * static_cast<R>(scalar),
            static_cast<R>(q.W()) * static_cast<R>(scalar)
        );
    }

    template <std::floating_point T, std::floating_point U>
    constexpr auto operator*(U scalar, const Quaternion<T>& q) -> QuaternionCommon<T, U> {
        return q * scalar;
    }

    template <std::floating_point T, std::floating_point U>
    constexpr auto operator/(const Quaternion<T>& q, U scalar) -> QuaternionCommon<T, U> {
        using R = std::common_type_t<T, U>;

        return QuaternionCommon<T, U>(
            static_cast<R>(q.X()) / static_cast<R>(scalar),
            static_cast<R>(q.Y()) / static_cast<R>(scalar),
            static_cast<R>(q.Z()) / static_cast<R>(scalar),
            static_cast<R>(q.W()) / static_cast<R>(scalar)
        );
    }

    template <std::floating_point T, std::floating_point U>
    constexpr auto operator*(const Quaternion<T>& q, const Vec3<U>& v) -> Vec3<std::common_type_t<T, U>> {
        using R = std::common_type_t<T, U>;

        const Quaternion<R> qr(static_cast<R>(q.X()), static_cast<R>(q.Y()), static_cast<R>(q.Z()), static_cast<R>(q.W()));
        const Vec3<R> vr{static_cast<R>(v[0]), static_cast<R>(v[1]), static_cast<R>(v[2])};

        return qr.RotateVector(vr);
    }

    template <std::floating_point T>
    constexpr T Dot(const Quaternion<T>& a, const Quaternion<T>& b) {
        return a.X() * b.X() + a.Y() * b.Y() + a.Z() * b.Z() + a.W() * b.W();
    }

    template <std::floating_point T>
    constexpr Quaternion<T> Lerp(const Quaternion<T>& a, const Quaternion<T>& b, T t) {
        return (a * (T{1} - t) + b * t).Normalized();
    }

    template <std::floating_point T>
    constexpr Quaternion<T> Slerp(const Quaternion<T>& a, const Quaternion<T>& b, T t) {
        Quaternion<T> end = b;
        T cosOmega = Dot(a, b);

        if (cosOmega < T{0}) {
            cosOmega = -cosOmega;
            end = -end;
        }

        constexpr T kEpsilon = static_cast<T>(1e-6);

        if (cosOmega > T{1} - kEpsilon) {
            return Lerp(a, end, t);
        }

        const T omega = std::acos(cosOmega);
        const T sinOmega = std::sin(omega);

        const T scaleA = std::sin((T{1} - t) * omega) / sinOmega;
        const T scaleB = std::sin(t * omega) / sinOmega;

        return (a * scaleA) + (end * scaleB);
    }

} // namespace Nexus

template <std::floating_point T>
struct std::formatter<Nexus::Quaternion<T>> {
    std::formatter<T> underlying;

    constexpr auto parse(std::format_parse_context& ctx) {
        return underlying.parse(ctx);
    }

    auto format(const Nexus::Quaternion<T>& obj, std::format_context& ctx) const {
        auto out = ctx.out();
        out = std::format_to(out, "(");
        out = underlying.format(obj.X(), ctx);
        out = std::format_to(out, ", ");
        out = underlying.format(obj.Y(), ctx);
        out = std::format_to(out, ", ");
        out = underlying.format(obj.Z(), ctx);
        out = std::format_to(out, ", ");
        out = underlying.format(obj.W(), ctx);
        return std::format_to(out, ")");
    }
};
