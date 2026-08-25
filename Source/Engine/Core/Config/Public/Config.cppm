// SPDX-License-Identifier: MIT

module;

#include <Platform/DLL/Export.hpp>

export module NE.Engine.Core.Config;

import std;

export namespace Nexus::Config {
    // --- Engine Info ---
    NEXUS_API constexpr std::string_view engineName = "Nexus Engine";
    NEXUS_API constexpr std::string_view engineVersion = "0.0.1";

    // --- Build Info ---
    enum class BuildMode { Debug, RelWithDebInfo, Release };

#if !defined(APP_BUILD_MODE)
#    error "APP_BUILD_MODE was not defined by the build system"
#endif

    NEXUS_API constexpr BuildMode buildMode =
#if APP_BUILD_MODE == 0
        BuildMode::Debug;
#elif APP_BUILD_MODE == 1
        BuildMode::RelWithDebInfo;
#else
        BuildMode::Release;
#endif

    NEXUS_API constexpr bool isDebug = (buildMode == BuildMode::Debug);

    NEXUS_API constexpr std::string_view buildType = buildMode == BuildMode::Debug            ? "Debug"
                                                     : buildMode == BuildMode::RelWithDebInfo ? "RelWithDebInfo"
                                                                                              : "Release";

    // --- Platform & Architecture ---
#if defined(_WIN32)
    NEXUS_API constexpr std::string_view platform = "Windows";
#elif defined(__APPLE__)
    NEXUS_API constexpr std::string_view platform = "Apple";
#elif defined(__linux__)
    NEXUS_API constexpr std::string_view platform = "Linux";
#elif defined(__unix__)
    NEXUS_API constexpr std::string_view platform = "Unix";
#else
    NEXUS_API constexpr std::string_view platform = "Unsupported";
#    error Unsupported platform
#endif

#if defined(_M_X64) || defined(__x86_64__)
    NEXUS_API constexpr std::string_view architecture = "x86_64";
    NEXUS_API constexpr int platformBits = 64;
#elif defined(_M_IX86) || defined(__i386__)
    NEXUS_API constexpr std::string_view architecture = "x86";
    NEXUS_API constexpr int platformBits = 32;
#elif defined(__aarch64__)
    NEXUS_API constexpr std::string_view architecture = "ARM64";
    NEXUS_API constexpr int platformBits = 64;
#elif defined(__arm__)
    NEXUS_API constexpr std::string_view architecture = "ARM";
    NEXUS_API constexpr int platformBits = 32;
#else
    NEXUS_API constexpr std::string_view architecture = "Unknown";
    NEXUS_API constexpr int platformBits = 0;
#    pragma message("Your architecture might not support the application.")
#endif

    // --- Compiler Info ---
#if defined(_MSC_VER)
    NEXUS_API constexpr std::string_view compiler = "MSVC";
    NEXUS_API constexpr int compilerVersion = _MSC_VER;
#elif defined(__clang__)
    NEXUS_API constexpr std::string_view compiler = "Clang";
    NEXUS_API constexpr int compilerVersion = (__clang_major__ * 100) + (__clang_minor__ * 10) + __clang_patchlevel__;
#elif defined(__GNUC__)
    NEXUS_API constexpr std::string_view compiler = "GCC";
    NEXUS_API constexpr int compilerVersion = (__GNUC__ * 100) + (__GNUC_MINOR__ * 10) + __GNUC_PATCHLEVEL__;
#else
    NEXUS_API constexpr std::string_view compiler = "Unknown";
    NEXUS_API constexpr int compilerVersion = 0;
#    pragma message("Your compiler might not work with this application.")
#endif

    // --- C++ version ---
#if defined(_MSVC_LANG)
#    define CPP_STD _MSVC_LANG
#else
#    define CPP_STD __cplusplus
#endif

#if CPP_STD > 202302L
    // The official macro value for C++26 is not yet finalized.
    NEXUS_API constexpr std::string_view cppStandard = "C++26 (Experimental)";
#elif CPP_STD == 202302L
    NEXUS_API constexpr std::string_view cppStandard = "C++23";
#else
    NEXUS_API constexpr std::string_view cppStandard = "Pre-C++23";
#    pragma message("Your C++ version might not support C++23 features used inside this engine.")
#endif

} // namespace Nexus::Config
