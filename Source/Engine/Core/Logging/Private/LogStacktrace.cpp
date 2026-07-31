// SPDX-License-Identifier: MIT

module NE.Engine.Core.Log;

namespace Nexus {
    std::string Logger::CaptureStacktrace() {
        std::string result;

        auto trace = std::stacktrace::current();
        for (const auto& entry : trace) {
            result += std::format("{}\n", entry);
        }

        return result;
    }
} // namespace Nexus
