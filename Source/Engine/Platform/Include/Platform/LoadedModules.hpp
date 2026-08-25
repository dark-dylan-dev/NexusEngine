// SPDX-License-Identifier: MIT

#pragma once

#include <Platform/DLL/Export.hpp>

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

namespace Nexus {
    struct NEXUS_API ModuleRange {
        uintptr_t base;
        uintptr_t end;
        std::string name;
    };

    NEXUS_API std::filesystem::path GetExecutablePath();
    NEXUS_API std::string GetExecutableName();
    NEXUS_API std::vector<ModuleRange> GetLoadedModules();
    NEXUS_API std::string ResolveModuleForAddress(uintptr_t addr, const std::vector<ModuleRange>& modules);
} // namespace Nexus
