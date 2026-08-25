// SPDX-License-Identifier: MIT

// clang-format off
#include <Platform/LoadedModules.hpp>

#include <mach-o/dyld.h>
#include <mach-o/loader.h>

#include <algorithm>
#include <cstdint>
// clang-format on

namespace Nexus {

    std::filesystem::path GetExecutablePath() {
        uint32_t size = 0;

        if (_NSGetExecutablePath(nullptr, &size) != -1)
            return {};

        std::vector<char> buffer(size);

        if (_NSGetExecutablePath(buffer.data(), &size) != 0)
            return {};

        return std::filesystem::path(buffer.data());
    }

    std::string GetExecutableName() {
        return GetExecutablePath().filename().string();
    }

    std::vector<ModuleRange> GetLoadedModules() {
        std::vector<ModuleRange> result;

        const uint32_t count = _dyld_image_count();

        result.reserve(count);

        for (uint32_t i = 0; i < count; ++i) {
            const mach_header* header = _dyld_get_image_header(i);

            const char* path = _dyld_get_image_name(i);

            if (!header || !path)
                continue;

            const intptr_t slide = _dyld_get_image_vmaddr_slide(i);

            uintptr_t base = UINTPTR_MAX;
            uintptr_t end = 0;

            if (header->magic == MH_MAGIC_64 || header->magic == MH_CIGAM_64) {
                const auto* header64 = reinterpret_cast<const mach_header_64*>(header);

                const auto* command = reinterpret_cast<const load_command*>(reinterpret_cast<const uint8_t*>(header64) +
                                                                            sizeof(mach_header_64));

                for (uint32_t j = 0; j < header64->ncmds; ++j) {
                    if (command->cmd == LC_SEGMENT_64) {
                        const auto* segment = reinterpret_cast<const segment_command_64*>(command);

                        const uintptr_t segmentBase = static_cast<uintptr_t>(segment->vmaddr + slide);

                        const uintptr_t segmentEnd = segmentBase + static_cast<uintptr_t>(segment->vmsize);

                        base = std::min(base, segmentBase);
                        end = std::max(end, segmentEnd);
                    }

                    command = reinterpret_cast<const load_command*>(reinterpret_cast<const uint8_t*>(command) +
                                                                    command->cmdsize);
                }
            } else {
                const auto* command = reinterpret_cast<const load_command*>(reinterpret_cast<const uint8_t*>(header) +
                                                                            sizeof(mach_header));

                for (uint32_t j = 0; j < header->ncmds; ++j) {
                    if (command->cmd == LC_SEGMENT) {
                        const auto* segment = reinterpret_cast<const segment_command*>(command);

                        const uintptr_t segmentBase = static_cast<uintptr_t>(segment->vmaddr + slide);

                        const uintptr_t segmentEnd = segmentBase + static_cast<uintptr_t>(segment->vmsize);

                        base = std::min(base, segmentBase);
                        end = std::max(end, segmentEnd);
                    }

                    command = reinterpret_cast<const load_command*>(reinterpret_cast<const uint8_t*>(command) +
                                                                    command->cmdsize);
                }
            }

            if (base == UINTPTR_MAX || base >= end)
                continue;

            std::string fullPath(path);

            const size_t slash = fullPath.find_last_of('/');

            std::string name = slash == std::string::npos ? fullPath : fullPath.substr(slash + 1);

            result.push_back({base, end, std::move(name)});
        }

        return result;
    }

} // namespace Nexus
