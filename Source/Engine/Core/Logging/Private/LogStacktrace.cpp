// SPDX-License-Identifier: MIT

module NE.Engine.Core.Log;

import NE.Engine.Core.Config;
import NE.Engine.Core.Types;

namespace Nexus {

    std::string Logger::CaptureStacktrace() {
        static constexpr std::string_view frameStart = "\n    ";

        auto ErrMsg = [](const std::string& message) { return std::string(frameStart) + message; };

#if __has_include(<stacktrace>)
        const auto trace = std::stacktrace::current();

        const usize skipFront = 3 + static_cast<usize>(s_CalledLogTrace.load(std::memory_order_acquire));
        usize skipBack = 3;

        if constexpr (Config::isDebug) {
            usize frmIdx = 0;
            for (const auto& entry : trace) {
                if (const auto name = std::filesystem::path(entry.source_file()).filename().generic_string();
                    name == "main.cpp") {
                    skipBack = trace.size() - frmIdx - 1;
                    break;
                }
                frmIdx++;
            }
        }

        if (trace.size() <= skipFront + skipBack) {
            return ErrMsg("Stacktrace too shallow to report");
        }

        const auto start = skipFront;
        const auto end = trace.size() - skipBack;

        std::string result;
        for (usize i = start; i < end; ++i) {
            const auto frameNumber = end - i;
            const auto lineStart = std::format("{}[#{}] ", frameStart, frameNumber);
            const auto& frame = trace[i];

            if constexpr (Config::isDebug) {
                result += std::format("{}Function: {}", lineStart, frame.description());
                result += std::format("{}Location: {}:{}", lineStart, frame.source_file(), frame.source_line());
            } else {
                const auto address = reinterpret_cast<uptr>(frame.native_handle());
                result += std::format("{}Address: {:#x}", lineStart, address);
            }
        }

        return result;
#else
        return ErrMsg("Unimplemented");
#endif
    }

} // namespace Nexus
