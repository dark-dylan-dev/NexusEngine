// SPDX-License-Identifier: MIT

module;

#include <csignal>
#include <cstdlib>
#include <ctime>
#include <version>

module NE.Engine.Core.Log;

namespace Nexus {
    void Logger::InstallCrashHandler() {
        std::set_terminate(&Logger::HandleTerminate);

#ifdef SIGABRT
        std::signal(SIGABRT, &Logger::HandleSignal);
#endif

#ifdef SIGFPE
        std::signal(SIGFPE, &Logger::HandleSignal);
#endif

#ifdef SIGILL
        std::signal(SIGILL, &Logger::HandleSignal);
#endif

#ifdef SIGINT
        std::signal(SIGINT, &Logger::HandleSignal);
#endif

#ifdef SIGSEGV
        std::signal(SIGSEGV, &Logger::HandleSignal);
#endif

#ifdef SIGTERM
        std::signal(SIGTERM, &Logger::HandleSignal);
#endif

#ifdef SIGBREAK
        std::signal(SIGBREAK, &Logger::HandleSignal);
#endif

#ifdef SIGHUP
        std::signal(SIGHUP, &Logger::HandleSignal);
#endif

#ifdef SIGQUIT
        std::signal(SIGQUIT, &Logger::HandleSignal);
#endif

#ifdef SIGTRAP
        std::signal(SIGTRAP, &Logger::HandleSignal);
#endif

#ifdef SIGBUS
        std::signal(SIGBUS, &Logger::HandleSignal);
#endif

#ifdef SIGSYS
        std::signal(SIGSYS, &Logger::HandleSignal);
#endif

#ifdef SIGPIPE
        std::signal(SIGPIPE, &Logger::HandleSignal);
#endif

#ifdef SIGALRM
        std::signal(SIGALRM, &Logger::HandleSignal);
#endif

#ifdef SIGUSR1
        std::signal(SIGUSR1, &Logger::HandleSignal);
#endif

#ifdef SIGUSR2
        std::signal(SIGUSR2, &Logger::HandleSignal);
#endif

#ifdef SIGCHLD
        std::signal(SIGCHLD, &Logger::HandleSignal);
#endif

#ifdef SIGCONT
        std::signal(SIGCONT, &Logger::HandleSignal);
#endif

#ifdef SIGSTOP
        std::signal(SIGSTOP, &Logger::HandleSignal);
#endif

#ifdef SIGTSTP
        std::signal(SIGTSTP, &Logger::HandleSignal);
#endif

#ifdef SIGTTIN
        std::signal(SIGTTIN, &Logger::HandleSignal);
#endif

#ifdef SIGTTOU
        std::signal(SIGTTOU, &Logger::HandleSignal);
#endif

#ifdef SIGURG
        std::signal(SIGURG, &Logger::HandleSignal);
#endif

#ifdef SIGVTALRM
        std::signal(SIGVTALRM, &Logger::HandleSignal);
#endif

#ifdef SIGPROF
        std::signal(SIGPROF, &Logger::HandleSignal);
#endif

#ifdef SIGXCPU
        std::signal(SIGXCPU, &Logger::HandleSignal);
#endif

#ifdef SIGXFSZ
        std::signal(SIGXFSZ, &Logger::HandleSignal);
#endif
    }

    void Logger::HandleSignal(const int signal) {
        std::string signalStr = "UNKNOWN";
        std::string reason = "Unhandled signal";

#ifdef SIGABRT
        if (signal == SIGABRT) {
            signalStr = "SIGABRT";
            reason += " (" + signalStr + ") Process was aborted";
        }
#endif

#ifdef SIGFPE
        if (signal == SIGFPE) {
            signalStr = "SIGFPE";
            reason += " (" + signalStr + ") Erroneous arithmetic operation";
        }
#endif

#ifdef SIGILL
        if (signal == SIGILL) {
            signalStr = "SIGILL";
            reason += " (" + signalStr + ") Illegal instruction encountered";
        }
#endif

#ifdef SIGINT
        if (signal == SIGINT) {
            signalStr = "SIGINT";
            reason += " (" + signalStr + ") Interrupt requested by the user";
        }
#endif

#ifdef SIGSEGV
        if (signal == SIGSEGV) {
            signalStr = "SIGSEGV";
            reason += " (" + signalStr + ") Invalid memory access";
        }
#endif

#ifdef SIGTERM
        if (signal == SIGTERM) {
            signalStr = "SIGTERM";
            reason += " (" + signalStr + ") Termination requested";
        }
#endif

#ifdef SIGBREAK
        if (signal == SIGBREAK) {
            signalStr = "SIGBREAK";
            reason += " (" + signalStr + ") Console break signal received";
        }
#endif

#ifdef SIGHUP
        if (signal == SIGHUP) {
            signalStr = "SIGHUP";
            reason += " (" + signalStr + ") Controlling terminal disconnected";
        }
#endif

#ifdef SIGQUIT
        if (signal == SIGQUIT) {
            signalStr = "SIGQUIT";
            reason += " (" + signalStr + ") Quit requested";
        }
#endif

#ifdef SIGTRAP
        if (signal == SIGTRAP) {
            signalStr = "SIGTRAP";
            reason += " (" + signalStr + ") Trace or breakpoint trap";
        }
#endif

#ifdef SIGBUS
        if (signal == SIGBUS) {
            signalStr = "SIGBUS";
            reason += " (" + signalStr + ") Bus error or invalid memory access";
        }
#endif

#ifdef SIGSYS
        if (signal == SIGSYS) {
            signalStr = "SIGSYS";
            reason += " (" + signalStr + ") Invalid system call";
        }
#endif

#ifdef SIGPIPE
        if (signal == SIGPIPE) {
            signalStr = "SIGPIPE";
            reason += " (" + signalStr + ") Broken pipe or closed connection";
        }
#endif

#ifdef SIGALRM
        if (signal == SIGALRM) {
            signalStr = "SIGALRM";
            reason += " (" + signalStr + ") Timer expired";
        }
#endif

#ifdef SIGUSR1
        if (signal == SIGUSR1) {
            signalStr = "SIGUSR1";
            reason += " (" + signalStr + ") User-defined signal 1";
        }
#endif

#ifdef SIGUSR2
        if (signal == SIGUSR2) {
            signalStr = "SIGUSR2";
            reason += " (" + signalStr + ") User-defined signal 2";
        }
#endif

#ifdef SIGCHLD
        if (signal == SIGCHLD) {
            signalStr = "SIGCHLD";
            reason += " (" + signalStr + ") Child process state changed";
        }
#endif

#ifdef SIGCONT
        if (signal == SIGCONT) {
            signalStr = "SIGCONT";
            reason += " (" + signalStr + ") Process continued";
        }
#endif

#ifdef SIGSTOP
        if (signal == SIGSTOP) {
            signalStr = "SIGSTOP";
            reason += " (" + signalStr + ") Process stop requested";
        }
#endif

#ifdef SIGTSTP
        if (signal == SIGTSTP) {
            signalStr = "SIGTSTP";
            reason += " (" + signalStr + ") Terminal stop requested";
        }
#endif

#ifdef SIGTTIN
        if (signal == SIGTTIN) {
            signalStr = "SIGTTIN";
            reason += " (" + signalStr + ") Background process attempted terminal input";
        }
#endif

#ifdef SIGTTOU
        if (signal == SIGTTOU) {
            signalStr = "SIGTTOU";
            reason += " (" + signalStr + ") Background process attempted terminal output";
        }
#endif

#ifdef SIGURG
        if (signal == SIGURG) {
            signalStr = "SIGURG";
            reason += " (" + signalStr + ") Urgent socket data available";
        }
#endif

#ifdef SIGVTALRM
        if (signal == SIGVTALRM) {
            signalStr = "SIGVTALRM";
            reason += " (" + signalStr + ") Virtual timer expired";
        }
#endif

#ifdef SIGPROF
        if (signal == SIGPROF) {
            signalStr = "SIGPROF";
            reason += " (" + signalStr + ") Profiling timer expired";
        }
#endif

#ifdef SIGXCPU
        if (signal == SIGXCPU) {
            signalStr = "SIGXCPU";
            reason += " (" + signalStr + ") CPU time limit exceeded";
        }
#endif

#ifdef SIGXFSZ
        if (signal == SIGXFSZ) {
            signalStr = "SIGXFSZ";
            reason += " (" + signalStr + ") File size limit exceeded";
        }
#endif

        WriteCrashReport(signalStr, reason, CaptureStacktrace());
        std::_Exit(signal);
    }

#define HANDLE_EXCEPTION(type)                                                                                         \
    catch (const type& e) {                                                                                            \
        WriteCrashReport("Unhandled C++ exception", "Type: " #type "\nMessage: " + std::string(e.what()),              \
                         CaptureStacktrace());                                                                         \
    }

    void Logger::HandleTerminate() {
        const std::exception_ptr exception = std::current_exception();

        if (!exception) {
            WriteCrashReport("std::terminate", "No active exception", CaptureStacktrace());

            std::_Exit(EXIT_FAILURE);
        }

        try {
            std::rethrow_exception(exception);
        }

        // std::logic_error hierarchy
        HANDLE_EXCEPTION(std::invalid_argument)
        HANDLE_EXCEPTION(std::domain_error)
        HANDLE_EXCEPTION(std::length_error)
        HANDLE_EXCEPTION(std::out_of_range)
        HANDLE_EXCEPTION(std::future_error)

        // std::runtime_error hierarchy
        HANDLE_EXCEPTION(std::filesystem::filesystem_error)
        HANDLE_EXCEPTION(std::ios_base::failure)
        HANDLE_EXCEPTION(std::system_error)
        HANDLE_EXCEPTION(std::range_error)
        HANDLE_EXCEPTION(std::overflow_error)
        HANDLE_EXCEPTION(std::underflow_error)
        HANDLE_EXCEPTION(std::regex_error)

        // C++20 chrono exceptions
#if defined(__cpp_lib_chrono) && __cpp_lib_chrono >= 201907L
        HANDLE_EXCEPTION(std::chrono::nonexistent_local_time)
        HANDLE_EXCEPTION(std::chrono::ambiguous_local_time)
#endif

        // C++20 formatting
#if defined(__cpp_lib_format) && __cpp_lib_format >= 201907L
        HANDLE_EXCEPTION(std::format_error)
#endif

        // RTTI / type utilities
        HANDLE_EXCEPTION(std::bad_typeid)
        HANDLE_EXCEPTION(std::bad_any_cast)
        HANDLE_EXCEPTION(std::bad_cast)
        HANDLE_EXCEPTION(std::bad_optional_access)
        HANDLE_EXCEPTION(std::bad_variant_access)
        HANDLE_EXCEPTION(std::bad_weak_ptr)
        HANDLE_EXCEPTION(std::bad_function_call)

        // C++23 expected
#if defined(__cpp_lib_expected) && __cpp_lib_expected >= 202202L
        HANDLE_EXCEPTION(std::bad_expected_access<void>)
#endif

        // Allocation
        HANDLE_EXCEPTION(std::bad_array_new_length)
        HANDLE_EXCEPTION(std::bad_alloc)

        // General
        HANDLE_EXCEPTION(std::bad_exception)

        // Base classes
        HANDLE_EXCEPTION(std::logic_error)
        HANDLE_EXCEPTION(std::runtime_error)
        HANDLE_EXCEPTION(std::exception)

        catch (...) {
            WriteCrashReport("Unhandled C++ exception", "Type: unknown\nMessage: Non-std::exception thrown",
                             CaptureStacktrace());
        }
        std::_Exit(EXIT_FAILURE);
    }

#undef HANDLE_EXCEPTION

    void Logger::WriteCrashReport(const std::string& reason, const std::string& details,
                                  const std::string& stacktrace) {
        std::ofstream crashLog("Crash.log", std::ios::out | std::ios::trunc);

        if (!crashLog.is_open()) {
            return;
        }

        const auto now = std::chrono::system_clock::now();
        const auto time = std::chrono::system_clock::to_time_t(now);

        std::tm localTime{};

#ifdef _WIN32
        localtime_s(&localTime, &time);
#else
        localtime_r(&time, &localTime);
#endif

        crashLog << "========================================\n"
                 << "           Nexus Engine Crash\n"
                 << "========================================\n"
                 << "Reason:     " << reason << '\n'
                 << "Time:       " << std::put_time(&localTime, "%Y-%m-%d %H:%M:%S") << "\n\n"
                 << "Details:\n"
                 << details << "\n\n"
                 << stacktrace << "\n========================================\n";
    }
} // namespace Nexus
