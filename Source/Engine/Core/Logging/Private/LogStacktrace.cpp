// SPDX-License-Identifier: MIT

module NE.Engine.Core.Log;

namespace Nexus {
    std::string Logger::CaptureStacktrace() {
#if !defined(__cpp_lib_stacktrace)
        // I would've liked to make a per OS solution when the stacktrace isn't available
        // But GCC just can't agree with that, see: https://github.com/mingw-w64/mingw-w64/issues/166
        return "";
#else
        std::string result;

        auto trace = std::stacktrace::current();
        for (const auto& entry : trace) {
            result += std::format("{}\n", entry);
        }

        return result;
#endif
    }
} // namespace Nexus
