// SPDX-License-Identifier: MIT

// clang-format off
#include <Platform/LoadedModules.hpp>

#include <format>
// clang-format on

namespace Nexus {
    std::string ResolveModuleForAddress(uintptr_t addr, const std::vector<ModuleRange>& modules) {
        for (const auto& mod : modules) {
            if (addr >= mod.base && addr < mod.end) {
                const uintptr_t offset = addr - mod.base;
                return std::format("{} + 0x{:x}", mod.name, offset);
            }
        }
        return "unknown";
    }
} // namespace Nexus
