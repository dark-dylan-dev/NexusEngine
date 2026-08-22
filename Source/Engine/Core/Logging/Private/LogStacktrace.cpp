// SPDX-License-Identifier: MIT

module NE.Engine.Core.Log;

import NE.Engine.Core.Config;

import cpptrace;

namespace Nexus {

    std::string Logger::CaptureStacktrace() {
        if constexpr (Config::buildMode == Config::BuildMode::Debug) {
            return cpptrace::formatter{}
                .addresses(cpptrace::formatter::address_mode::none)
                .break_before_filename()
                .symbols(cpptrace::formatter::symbol_mode::pretty)
                .format(cpptrace::generate_trace());

        } else if constexpr (Config::buildMode == Config::BuildMode::RelWithDebInfo) {
            return cpptrace::formatter{}
                .addresses(cpptrace::formatter::address_mode::object)
                .break_before_filename()
                .symbols(cpptrace::formatter::symbol_mode::pretty)
                .format(cpptrace::generate_trace());

        } else {
            return cpptrace::formatter{}
                .addresses(cpptrace::formatter::address_mode::object)
                .symbols(cpptrace::formatter::symbol_mode::pruned)
                .format(cpptrace::generate_trace());
        }
    }

} // namespace Nexus
