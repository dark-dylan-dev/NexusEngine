// SPDX-License-Identifier: MIT

module;

#include <csignal>

module NE.Engine.Core.Log;

namespace Nexus {
    void Logger::InstallCrashHandler() {
        std::signal(SIGSEGV, HandleCrash);
        std::signal(SIGABRT, HandleCrash);
        std::signal(SIGINT, HandleCrash);
    }

    void Logger::HandleCrash(int signal) {
        std::string signalStr = "Unhandled signal";
        switch (signal) {
            case SIGSEGV:
                signalStr = "SIGSEGV";
                break;
            case SIGABRT:
                signalStr = "SIGABRT";
                break;
            case SIGINT:
                signalStr = "SIGINT";
                break;
            default:
                break;
        }
        std::cerr << "Crash detected. Signal: " << signalStr << "\n";
        std::cerr << CaptureStacktrace();

        std::exit(signal);
    }
} // namespace Nexus
