// SPDX-License-Identifier: MIT

#include "Utils.hpp"

#if defined(_WIN32)
#    include <windows.h>
#else
#    include <cstdlib>
#    include <unistd.h>
#endif

#include <format>

namespace Nexus {
    bool EnableAnsiColors() {
#if defined(_WIN32)
        const HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD mode;
        if (!GetConsoleMode(h, &mode))
            return false;
        if (!SetConsoleMode(h, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING))
            return false;
        return true;
#else
        const char* term = std::getenv("TERM");
        return isatty(STDOUT_FILENO) && term && std::string(term) != "dumb";
#endif
    }

    std::string ANSI(int code, bool bold) {
        return std::format("\x1B[{}{}m", bold ? "1;" : "", code);
    }

    std::string ANSI_RGB(int r, int g, int b, bool bold) {
        return std::format("\x1B[{}38;2;{};{};{}m", bold ? "1;" : "", r, g, b);
    }
} // namespace Nexus
