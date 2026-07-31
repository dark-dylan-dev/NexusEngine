// SPDX-License-Identifier: MIT

module NE.Engine.Core.Log;

import NE.Engine.Core.Config;
import NE.Engine.Core.Log.RingBuffer;

import std;

namespace Nexus {

    Logger::Logger(const std::filesystem::path& path, bool enableConsole) {
        InstallCrashHandler();
        Init(path.generic_string(), enableConsole);
        Log(LogLevel::Info, "Logging system initialized");
    }

    Logger::~Logger() {
        Log(LogLevel::Info, "Logger system shutdown");
        Shutdown();
    }

    void Logger::Init(const std::filesystem::path& filePath, bool enableConsole) {
        m_File.open(filePath, std::ios::out | std::ios::trunc);
        m_EnableConsole = enableConsole;
        m_Running = true;

        m_BackgroundThread = std::jthread([this] {
            LogEntry entry;
            usize emptySpins = 0;

            while (m_Running) {
                if (m_Buffer.TryPop(entry)) {
                    m_File.write(entry.Formatted.data(), static_cast<std::streamsize>(entry.Formatted.size()));
                    if (m_EnableConsole)
                        std::print("{}", entry.Formatted);

                    emptySpins = 0;
                } else {
                    ++emptySpins;

                    if (emptySpins < 1000) {
                        // Spin aggressively
                    } else if (emptySpins < 5000) {
                        std::this_thread::yield();
                    } else {
                        std::this_thread::sleep_for(std::chrono::microseconds(100));
                    }
                }
            }
        });
    }

    // Log
    void Logger::Log(LogLevel level, std::string_view message) {
        m_Buffer.Insert({.Formatted = Format(level, message)});
    }

    // Flush
    void Logger::Flush() {
        LogEntry msg;

        while (m_Buffer.TryPop(msg)) {
            WriteToFile(msg);
            if (m_EnableConsole)
                WriteToConsole(msg);
        }

        m_File.flush();
    }

    // Shutdown
    void Logger::Shutdown() {
        m_Running = false;
        if (m_BackgroundThread.joinable())
            m_BackgroundThread.join();

        Flush();
        m_File.close();
    }

    static const char* ToString(LogLevel level) {
        switch (level) {
            case LogLevel::Trace:
                return "TRACE";
            case LogLevel::Debug:
                return "DEBUG";
            case LogLevel::Info:
                return "INFO ";
            case LogLevel::Warn:
                return "WARN ";
            case LogLevel::Error:
                return "ERROR";
            case LogLevel::Fatal:
                return "FATAL";
            default:
                return "UNK  ";
        }
    }

    static const std::string& CachedThreadId() {
        thread_local std::string id = std::format("{:>5}", std::this_thread::get_id());
        return id;
    }

    // Output

    std::string Logger::Format(LogLevel level, std::string_view message) {
        using namespace std::chrono;

        const auto now = system_clock::now();
        const auto seconds = floor<std::chrono::seconds>(now);

        if (seconds != m_LastSecond) {
            const auto tt = system_clock::to_time_t(seconds);
            const std::tm* local = std::localtime(&tt);

            char buffer[9];
            std::strftime(buffer, sizeof(buffer), "%H:%M:%S", local);
            m_CachedTime = buffer;
            m_LastSecond = seconds;
        }

        thread_local std::string tl_FormatBuffer;
        tl_FormatBuffer.clear();

        if (tl_FormatBuffer.capacity() < 256) {
            tl_FormatBuffer.reserve(256);
        }

        std::format_to(std::back_inserter(tl_FormatBuffer), "[thread:{}] [{}] [{}] {}\n", CachedThreadId(),
                       m_CachedTime, ToString(level), message);

        return tl_FormatBuffer;
    }

    void Logger::WriteToFile(LogEntry& entry) {
        m_File.write(entry.Formatted.data(), static_cast<std::streamsize>(entry.Formatted.size()));
    }

    void Logger::WriteToConsole(LogEntry& entry) {
        std::print("{}", entry.Formatted);
    }
} // namespace Nexus
