// SPDX-License-Identifier: MIT

#ifndef NOMINMAX
#    define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#endif

// clang-format off
#include <Platform/LoadedModules.hpp>

#include <windows.h>
#include <psapi.h>

#include <algorithm>
// clang-format on

namespace Nexus {

    std::filesystem::path GetExecutablePath() {
        std::vector<char> buffer(MAX_PATH);

        for (;;) {
            const DWORD length = GetModuleFileNameA(nullptr, buffer.data(), static_cast<DWORD>(buffer.size()));

            if (length == 0)
                return {};

            if (length < buffer.size() - 1) {
                return std::filesystem::path(std::string(buffer.data(), length));
            }

            buffer.resize(buffer.size() * 2);
        }
    }

    std::string GetExecutableName() {
        return GetExecutablePath().filename().string();
    }

    std::vector<ModuleRange> GetLoadedModules() {
        std::vector<ModuleRange> result;

        HANDLE process = GetCurrentProcess();

        HMODULE modules[1024];
        DWORD needed = 0;

        if (!EnumProcessModules(process, modules, sizeof(modules), &needed)) {
            return result;
        }

        const size_t count = std::min<size_t>(needed / sizeof(HMODULE), std::size(modules));

        result.reserve(count);

        for (size_t i = 0; i < count; ++i) {
            MODULEINFO info{};

            if (!GetModuleInformation(process, modules[i], &info, sizeof(info))) {
                continue;
            }

            char nameBuffer[MAX_PATH];

            const DWORD length = GetModuleFileNameExA(process, modules[i], nameBuffer, MAX_PATH);

            if (length == 0)
                continue;

            std::string fullPath(nameBuffer, length);

            const size_t slash = fullPath.find_last_of("\\/");

            std::string name = slash == std::string::npos ? fullPath : fullPath.substr(slash + 1);

            const uintptr_t base = reinterpret_cast<uintptr_t>(info.lpBaseOfDll);

            const uintptr_t end = base + static_cast<uintptr_t>(info.SizeOfImage);

            result.push_back({base, end, std::move(name)});
        }

        return result;
    }

} // namespace Nexus
