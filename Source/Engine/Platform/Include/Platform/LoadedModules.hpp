// SPDX-License-Identifier: MIT

#pragma once

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

namespace Nexus {
    struct ModuleRange {
        uintptr_t base;
        uintptr_t end;
        std::string name;
    };

    std::filesystem::path GetExecutablePath();
    std::string GetExecutableName();
    std::vector<ModuleRange> GetLoadedModules();
    std::string ResolveModuleForAddress(uintptr_t addr, const std::vector<ModuleRange>& modules);
} // namespace Nexus
