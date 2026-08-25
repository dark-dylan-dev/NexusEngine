// SPDX-License-Identifier: MIT

module;

#include <Platform/LoadedModules.hpp>

module NE.Engine.Core.Log;

import NE.Engine.Core.Config;

import cpptrace;

namespace Nexus {

    std::string Logger::CaptureStacktrace() {
        // Skipped functions:
        // 0. Log (only when LogTrace is called)
        // 1. Format
        // 2. CaptureStacktace
        // 3. cpptrace::formatter
        const auto skip = 3 + s_CalledLogTrace.load(std::memory_order_acquire);
        if constexpr (Config::buildMode == Config::BuildMode::Debug) {
            return cpptrace::formatter{}
                .addresses(cpptrace::formatter::address_mode::none)
                .break_before_filename()
                .transform([](cpptrace::stacktrace_frame frame) {
                    frame.filename = std::filesystem::exists(frame.filename) ? frame.filename : "";
                    return frame;
                })
                .snippets(true)
                .symbols(cpptrace::formatter::symbol_mode::pretty)
                .format(cpptrace::generate_trace(skip));
        } else if constexpr (Config::buildMode == Config::BuildMode::RelWithDebInfo) {
            auto modules = GetLoadedModules();
            return cpptrace::formatter{}
                .addresses(cpptrace::formatter::address_mode::object)
                .break_before_filename()
                .transform([modules](cpptrace::stacktrace_frame frame) {
                    frame.filename = frame.line == cpptrace::nullable<uint32>::null()
                                         ? ResolveModuleForAddress(frame.raw_address, modules)
                                     : std::filesystem::exists(frame.filename) ? frame.filename
                                                                               : "";
                    frame.symbol = cpptrace::prettify_symbol(frame.symbol);
                    return frame;
                })
                .symbols(cpptrace::formatter::symbol_mode::pretty)
                .format(cpptrace::generate_trace(skip));
        } else {
            auto modules = GetLoadedModules();
            return cpptrace::formatter{}
                .addresses(cpptrace::formatter::address_mode::object)
                .transform([modules](cpptrace::stacktrace_frame frame) {
                    frame.filename = "";
                    frame.symbol = ResolveModuleForAddress(frame.raw_address, modules);
                    return frame;
                })
                .format(cpptrace::generate_trace(skip));
        }
    }

} // namespace Nexus
