// SPDX-License-Identifier: MIT

#pragma once

#include <string>

namespace Nexus {
    bool EnableAnsiColors();
    std::string ANSI(int code, bool bold = false);
    std::string ANSI_RGB(int r, int g, int b, bool bold = false);
} // namespace Nexus
