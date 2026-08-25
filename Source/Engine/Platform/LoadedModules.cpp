// SPDX-License-Identifier: MIT

#if defined(_WIN32)
#    ifndef NOMINMAX
#        define NOMINMAX
#    endif
#    ifndef WIN32_LEAN_AND_MEAN
#        define WIN32_LEAN_AND_MEAN
#    endif
// clang-format off
#    include <windows.h>
#    include <psapi.h>
// clang-format on
#elif defined(__APPLE__)
#    include <mach-o/dyld.h>
#    include <mach-o/getsect.h>
#elif defined(__linux__)
#    include <limits.h>
#    include <link.h>
#    include <unistd.h>
#endif

#include <Platform/LoadedModules.hpp>
#include <format>

namespace Nexus {
#if defined(_WIN32)

    std::vector<ModuleRange> GetLoadedModules() {
        std::vector<ModuleRange> result;

        HMODULE modules[1024];
        DWORD needed;
        HANDLE process = GetCurrentProcess();

        if (!EnumProcessModules(process, modules, sizeof(modules), &needed))
            return result;

        size_t count = needed / sizeof(HMODULE);
        for (size_t i = 0; i < count; ++i) {
            MODULEINFO info;
            if (!GetModuleInformation(process, modules[i], &info, sizeof(info)))
                continue;

            char nameBuf[MAX_PATH];
            DWORD len = GetModuleFileNameExA(process, modules[i], nameBuf, MAX_PATH);
            std::string fullPath = (len > 0) ? std::string(nameBuf, len) : "unknown";

            size_t slash = fullPath.find_last_of("\\/");
            std::string name = (slash == std::string::npos) ? fullPath : fullPath.substr(slash + 1);

            uintptr_t base = reinterpret_cast<uintptr_t>(info.lpBaseOfDll);
            result.push_back({base, base + info.SizeOfImage, std::move(name)});
        }

        return result;
    }

#elif defined(__APPLE__)

    std::vector<ModuleRange> GetLoadedModules() {
        std::vector<ModuleRange> result;

        uint32_t count = _dyld_image_count();
        result.reserve(count);

        for (uint32_t i = 0; i < count; ++i) {
            const char* path = _dyld_get_image_name(i);
            if (!path)
                continue;

            const struct mach_header* header = _dyld_get_image_header(i);
            intptr_t slide = _dyld_get_image_vmaddr_slide(i);

            unsigned long textSize = 0;
            const uint8_t* base =
                getsegmentdata(reinterpret_cast<const struct mach_header_64*>(header), "__TEXT", &textSize);

            uintptr_t baseAddr = reinterpret_cast<uintptr_t>(header);

            std::string fullPath(path);
            size_t slash = fullPath.find_last_of('/');
            std::string name = (slash == std::string::npos) ? fullPath : fullPath.substr(slash + 1);

            result.push_back({baseAddr, baseAddr + textSize, std::move(name)});
            (void)slide;
            (void)base;
        }

        return result;
    }

#elif defined(__linux__)

    std::filesystem::path GetExecutablePath()
    {
        char buffer[4096];

        const ssize_t length =
            readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);

        if (length == -1)
            return {};

        buffer[length] = '\0';
        return std::filesystem::path(buffer);
    }

    namespace {

        int PhdrCallback(struct dl_phdr_info* info, size_t /*size*/, void* data) {
            auto* result = reinterpret_cast<std::vector<Nexus::ModuleRange>*>(data);

            uintptr_t base = info->dlpi_addr;
            uintptr_t maxEnd = base;

            for (int i = 0; i < info->dlpi_phnum; ++i) {
                const auto& phdr = info->dlpi_phdr[i];
                if (phdr.p_type != PT_LOAD)
                    continue;

                uintptr_t segEnd = base + phdr.p_vaddr + phdr.p_memsz;
                if (segEnd > maxEnd)
                    maxEnd = segEnd;
            }

            std::string fullPath =
                (info->dlpi_name && info->dlpi_name[0] != '\0') ? info->dlpi_name : GetExecutablePath().string();

            size_t slash = fullPath.find_last_of('/');
            std::string name = (slash == std::string::npos) ? fullPath : fullPath.substr(slash + 1);

            result->push_back({base, maxEnd, std::move(name)});
            return 0;
        }

    } // namespace

    std::vector<ModuleRange> GetLoadedModules() {
        std::vector<ModuleRange> result;
        dl_iterate_phdr(PhdrCallback, &result);
        return result;
    }

#endif

    std::string ResolveModuleForAddress(uintptr_t addr, const std::vector<ModuleRange>& modules) {
        for (const auto& mod : modules) {
            if (addr >= mod.base && addr < mod.end) {
                uintptr_t offset = addr - mod.base;
                return std::format("{} + 0x{:x}", mod.name, offset);
            }
        }
        return "unknown";
    }
} // namespace Nexus
