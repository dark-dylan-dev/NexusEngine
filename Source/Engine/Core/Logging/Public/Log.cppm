// SPDX-License-Identifier: MIT

module;

#include <Platform/DLL/Export.hpp>

export module NE.Engine.Core.Log;

export import NE.Engine.Core.Log.RingBuffer;
import NE.Engine.Core.Types;

import std;

export namespace Nexus {
    enum class LogLevel { Trace, Debug, Info, Warn, Error, Fatal };

    struct NEXUS_API LogEntry {
        std::string Formatted;
        LogLevel Level;
    };

    class NEXUS_API Logger {
    public:
        explicit Logger(const std::filesystem::path& path = std::filesystem::path("Log.log"),
                        bool enableConsole = true);
        ~Logger();

        void Log(LogLevel level, std::string_view message);
        void LogTrace(std::string_view message);
        void LogDebug(std::string_view message);
        void LogInfo(std::string_view message);
        void LogWarn(std::string_view message);
        void LogError(std::string_view message);
        void LogFatal(std::string_view message);

        void Flush();

    private:
        void Init(const std::filesystem::path& filePath, bool enableConsole);
        void Shutdown();

        // Crash handling
        static void InstallCrashHandler();
        static void HandleSignal(int signal);
        static void HandleTerminate();
        static void WriteCrashReport(const std::string& reason, const std::string& details,
                                     const std::string& stacktrace);

        void WriteToFile(LogEntry& entry);
        void WriteToConsole(LogEntry& entry);

        std::string Format(LogLevel level, std::string_view message);

        static std::string CaptureStacktrace();

    private:
        std::unique_ptr<RingBuffer<LogEntry, 4096>> m_Buffer = nullptr;

        std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds> m_LastSecond{};
        std::string m_CachedTime{};

        std::unique_ptr<std::ofstream> m_File = nullptr;
        bool m_EnableConsole{true};
        bool m_HasColors{false};

        std::atomic<bool> m_Running{false};
        std::jthread m_BackgroundThread;

        std::atomic<bool> m_FlushRequested{false};
        std::atomic<uint64> m_FlushGeneration{0};
        std::mutex m_FlushMutex;
        std::condition_variable m_FlushCv;

        static inline std::atomic<bool> s_CalledLogTrace{false};
    };

} // namespace Nexus
