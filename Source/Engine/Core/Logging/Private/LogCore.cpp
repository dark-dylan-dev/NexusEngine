// SPDX-License-Identifier: MIT

module;

#include <ctime>

module NE.Engine.Core.Log;

import NE.Engine.Core.Config;
import NE.Engine.Core.Log.RingBuffer;

import std;

namespace Nexus {

    Logger::Logger(const std::filesystem::path& path, bool enableConsole) {
        InstallCrashHandler();
        Init(path.generic_string(), enableConsole);
        LogInfo("Logging system initialized");
    }

    Logger::~Logger() {
        LogInfo("Logger system shutdown");
        Shutdown();
    }

    void Logger::Init(const std::filesystem::path& filePath, bool enableConsole) {
        m_File = std::make_unique<std::ofstream>();
        m_File->open(filePath, std::ios::out | std::ios::trunc);
        m_EnableConsole = enableConsole;
        m_Running.store(true, std::memory_order_release);
        m_Buffer = std::make_unique<RingBuffer<LogEntry, 4096>>();

        m_BackgroundThread = std::jthread([this] {
            LogEntry entry;
            usize emptySpins = 0;

            while (m_Running.load(std::memory_order_acquire)) {
                if (m_Buffer->Pop(entry)) {
                    m_File->write(entry.Formatted.data(), static_cast<isize>(entry.Formatted.size()));
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

                if (m_FlushRequested.load(std::memory_order_acquire)) {
                    while (m_Buffer->Pop(entry)) {
                        m_File->write(entry.Formatted.data(), static_cast<isize>(entry.Formatted.size()));
                        if (m_EnableConsole)
                            std::print("{}", entry.Formatted);
                    }

                    m_File->flush();

                    {
                        std::lock_guard lock(m_FlushMutex);
                        m_FlushRequested.store(false, std::memory_order_release);
                        m_FlushGeneration.fetch_add(1, std::memory_order_release);
                    }
                    m_FlushCv.notify_all();
                }
            }
        });
    }

    // Log
    void Logger::Log(LogLevel level, std::string_view message) {
        while (!m_Buffer->Push({.Formatted = Format(level, message)})) {
            std::this_thread::yield();
        }
    }

    void Logger::LogTrace(std::string_view message) {
        Log(LogLevel::Trace, message);
    }

    void Logger::LogDebug(std::string_view message) {
        Log(LogLevel::Debug, message);
    }

    void Logger::LogInfo(std::string_view message) {
        Log(LogLevel::Info, message);
    }

    void Logger::LogWarn(std::string_view message) {
        Log(LogLevel::Warn, message);
    }

    void Logger::LogError(std::string_view message) {
        Log(LogLevel::Error, message);
    }

    void Logger::LogFatal(std::string_view message) {
        Log(LogLevel::Fatal, message);
    }

    // Flush
    void Logger::Flush() {
        uint64 generationBefore = m_FlushGeneration.load(std::memory_order_acquire);

        m_FlushRequested.store(true, std::memory_order_release);

        std::unique_lock lock(m_FlushMutex);
        m_FlushCv.wait(lock, [&] { return m_FlushGeneration.load(std::memory_order_acquire) != generationBefore; });
    }

    // Shutdown
    void Logger::Shutdown() {
        Flush();

        m_Running.store(false, std::memory_order_release);
        if (m_BackgroundThread.joinable())
            m_BackgroundThread.join();

        m_File->close();
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
            std::tm local{};

#if !defined(_WIN32)
            localtime_r(&tt, &local);
#else
            localtime_s(&local, &tt);
#endif

            char buffer[9];
            std::strftime(buffer, sizeof(buffer), "%H:%M:%S", &local);
            m_CachedTime = buffer;
            m_LastSecond = seconds;
        }

        return std::format("[thread:{}] [{}] [{}] {}\n", CachedThreadId(), m_CachedTime, ToString(level), message);
    }

    void Logger::WriteToFile(LogEntry& entry) {
        m_File->write(entry.Formatted.data(), static_cast<isize>(entry.Formatted.size()));
    }

    void Logger::WriteToConsole(LogEntry& entry) {
        std::print("{}", entry.Formatted);
    }
} // namespace Nexus
