// SPDX-License-Identifier: MIT

export module NE.Engine.Core.Log;

export import NE.Engine.Core.Log.RingBuffer;
import NE.Engine.Core.Types;

import std;

export namespace Nexus {
    class Logger {
    public:
        explicit Logger(const std::filesystem::path& path = std::filesystem::path("Log.log"),
                        bool enableConsole = true);
        ~Logger();

        void Init(const std::filesystem::path& filePath, bool enableConsole);

        void Log(LogLevel level, std::string_view message);
        void LogTrace(std::string_view message);
        void LogDebug(std::string_view message);
        void LogInfo(std::string_view message);
        void LogWarn(std::string_view message);
        void LogError(std::string_view message);
        void LogFatal(std::string_view message);

        void Flush();

        void Shutdown();

        // Crash handling
        static void InstallCrashHandler();
        static void HandleCrash(int signal);

    private:
        void WriteToFile(LogEntry& entry);
        void WriteToConsole(LogEntry& entry);

        std::string Format(LogLevel level, std::string_view message);

        static std::string CaptureStacktrace();

    private:
        RingBuffer<LogEntry, 4096>* m_Buffer = nullptr;

        std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds> m_LastSecond{};
        std::string m_CachedTime{};

        std::ofstream* m_File = nullptr;
        bool m_EnableConsole{true};

        std::atomic<bool> m_Running{false};
        std::jthread m_BackgroundThread;
    };

} // namespace Nexus
