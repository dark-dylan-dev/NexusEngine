// SPDX-License-Identifier: MIT

// clang-format off
#include <Platform/LoadedModules.hpp>

#include <link.h>
#include <unistd.h>

#include <algorithm>
#include <climits>
// clang-format on

namespace Nexus {

    std::filesystem::path GetExecutablePath() {
        std::vector<char> buffer(4096);

        for (;;) {
            const ssize_t length = readlink("/proc/self/exe", buffer.data(), buffer.size());

            if (length < 0)
                return {};

            if (static_cast<size_t>(length) < buffer.size()) {
                return std::filesystem::path(buffer.data(), static_cast<size_t>(length));
            }

            buffer.resize(buffer.size() * 2);
        }
    }

    std::string GetExecutableName() {
        return GetExecutablePath().filename().string();
    }

    namespace {

        int PhdrCallback(struct dl_phdr_info* info, size_t /*size*/, void* data) {
            auto* result = static_cast<std::vector<Nexus::ModuleRange>*>(data);

            uintptr_t base = UINTPTR_MAX;
            uintptr_t end = 0;

            for (int i = 0; i < info->dlpi_phnum; ++i) {
                const ElfW(Phdr) & phdr = info->dlpi_phdr[i];

                if (phdr.p_type != PT_LOAD)
                    continue;

                const uintptr_t segmentBase = static_cast<uintptr_t>(info->dlpi_addr + phdr.p_vaddr);

                const uintptr_t segmentEnd = segmentBase + static_cast<uintptr_t>(phdr.p_memsz);

                base = std::min(base, segmentBase);
                end = std::max(end, segmentEnd);
            }

            if (base == UINTPTR_MAX || base >= end)
                return 0;

            std::string fullPath;

            if (info->dlpi_name && info->dlpi_name[0] != '\0')
                fullPath = info->dlpi_name;
            else
                fullPath = GetExecutablePath().string();

            const size_t slash = fullPath.find_last_of('/');

            std::string name = slash == std::string::npos ? fullPath : fullPath.substr(slash + 1);

            result->push_back({base, end, std::move(name)});

            return 0;
        }

    } // namespace

    std::vector<ModuleRange> GetLoadedModules() {
        std::vector<ModuleRange> result;

        dl_iterate_phdr(PhdrCallback, &result);

        return result;
    }

} // namespace Nexus
