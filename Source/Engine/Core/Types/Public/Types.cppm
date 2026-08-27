// SPDX-License-Identifier: MIT

export module NE.Engine.Core.Types;

import std;

export namespace Nexus {
    // Unsigned integers
    using uint8 = std::uint8_t;
    using uint16 = std::uint16_t;
    using uint32 = std::uint32_t;
    using uint64 = std::uint64_t;

    // Signed integers
    using int8 = std::int8_t;
    using int16 = std::int16_t;
    using int32 = std::int32_t;
    using int64 = std::int64_t;

    // Floating point
    using float32 = float;
    using float64 = double;

    // Size-related
    using usize = std::size_t;
    using isize = std::ptrdiff_t;

    // Pointer-sized integers
    using uptr = std::uintptr_t;
    using iptr = std::intptr_t;

    // Byte type
    using byte = std::byte;

    template <typename T, typename... Excluded>
    concept NoneOf = (!std::same_as<std::remove_cv_t<T>, Excluded> && ...);

    template <typename T>
    concept Numeric = std::is_arithmetic_v<T> &&
                      NoneOf<T, bool, char, signed char, unsigned char, wchar_t, char8_t, char16_t, char32_t>;

    template <typename T>
    struct EnableFlagOperators : std::false_type {};

    template <typename T>
    concept FlagsEnum = EnableFlagOperators<T>::value && std::is_scoped_enum_v<T>;

    template <FlagsEnum T>
    constexpr T operator|(T a, T b) {
        return static_cast<T>(std::to_underlying(a) | std::to_underlying(b));
    }
    template <FlagsEnum T>
    constexpr T operator&(T a, T b) {
        return static_cast<T>(std::to_underlying(a) & std::to_underlying(b));
    }
    template <FlagsEnum T>
    constexpr T operator^(T a, T b) {
        return static_cast<T>(std::to_underlying(a) ^ std::to_underlying(b));
    }
    template <FlagsEnum T>
    constexpr T operator~(T a) {
        return static_cast<T>(~std::to_underlying(a));
    }
    template <FlagsEnum T>
    constexpr T& operator|=(T& a, T b) {
        return a = a | b;
    }
    template <FlagsEnum T>
    constexpr T& operator&=(T& a, T b) {
        return a = a & b;
    }
    template <FlagsEnum T>
    constexpr T& operator^=(T& a, T b) {
        return a = a ^ b;
    }

    template <FlagsEnum T>
    constexpr bool HasFlag(T value, T flag) {
        return (value & flag) == flag;
    }

    // idea from: https://stackoverflow.com/a/56766138
    // made it simpler and consteval
    template <typename T>
    consteval auto TypeName() {
#if defined(__clang__)
        constexpr std::string_view prefix = "[T = ";
        constexpr std::string_view suffix = "]";
        constexpr std::string_view func = __PRETTY_FUNCTION__;
#elif defined(__GNUC__)
        constexpr std::string_view prefix = "[with T = ";
        constexpr std::string_view suffix = "]";
        constexpr std::string_view func = __PRETTY_FUNCTION__;
#elif defined(_MSC_VER)
        constexpr std::string_view prefix = "TypeName<";
        constexpr std::string_view suffix = ">(void)";
        constexpr std::string_view func = __FUNCSIG__;
#endif
        constexpr auto start = func.find(prefix) + prefix.size();
        constexpr auto end = func.rfind(suffix);
        return func.substr(start, end - start);
    }

} // namespace Nexus
