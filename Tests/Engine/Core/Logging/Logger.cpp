// SPDX-License-Identifier: MIT

#include <gtest/gtest.h>

import NE.Engine.Core.Log;
import NE.Engine.Core.Types;

import std;

namespace {
    const std::filesystem::path logPath = "TestLogger.log";

    class LoggerTest : public ::testing::Test {
    protected:
        LoggerTest() : m_Logger(logPath, false) {}

        static void SetUpTestSuite() {
            std::filesystem::remove(logPath);
        }

        static void TearDownTestSuite() {
            std::filesystem::remove(logPath);
        }

        static std::string ReadLogFile() {
            if (!std::filesystem::exists(logPath))
                return {};

            std::ifstream file(logPath);
            if (!file.is_open())
                return {};

            return {std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};
        }

        Nexus::Logger m_Logger;
    };
} // namespace

TEST_F(LoggerTest, Creation) {
    // Checks if m_Logger created the file
    EXPECT_TRUE(std::filesystem::exists(logPath));
}

TEST_F(LoggerTest, Destruction) {
    {
        Nexus::Logger logger(logPath, false);
        logger.LogInfo("Test message");
        logger.Flush();
    }

    EXPECT_TRUE(std::filesystem::exists(logPath));

    EXPECT_TRUE(ReadLogFile().contains("Test message"));
}

TEST_F(LoggerTest, LogTrace) {
    m_Logger.LogTrace("Trace message");
    m_Logger.Flush();

    EXPECT_TRUE(ReadLogFile().contains("Trace message"));
    EXPECT_TRUE(ReadLogFile().contains("Stack trace (most recent call first):"));
}

TEST_F(LoggerTest, LogDebug) {
    m_Logger.LogDebug("Debug message");
    m_Logger.Flush();

    EXPECT_TRUE(ReadLogFile().contains("Debug message"));
}

TEST_F(LoggerTest, LogInfo) {
    m_Logger.LogInfo("Info message");
    m_Logger.Flush();

    EXPECT_TRUE(ReadLogFile().contains("Info message"));
}

TEST_F(LoggerTest, LogWarn) {
    m_Logger.LogWarn("Warning message");
    m_Logger.Flush();

    EXPECT_TRUE(ReadLogFile().contains("Warning message"));
}

TEST_F(LoggerTest, LogError) {
    m_Logger.LogError("Error message");
    m_Logger.Flush();

    EXPECT_TRUE(ReadLogFile().contains("Error message"));
}

TEST_F(LoggerTest, LogFatal) {
    m_Logger.LogFatal("Fatal message");
    m_Logger.Flush();

    EXPECT_TRUE(ReadLogFile().contains("Fatal message"));
}

TEST_F(LoggerTest, LogGeneric) {
    m_Logger.Log(Nexus::LogLevel::Info, "Generic message");
    m_Logger.Flush();

    EXPECT_TRUE(ReadLogFile().contains("Generic message"));
}

TEST_F(LoggerTest, MultipleMessages) {
    m_Logger.LogInfo("First message");
    m_Logger.LogWarn("Second message");
    m_Logger.LogError("Third message");
    m_Logger.Flush();

    const auto content = ReadLogFile();

    const auto firstPresent = content.contains("First message");
    const auto secondPresent = content.contains("Second message");
    const auto thirdPresent = content.contains("Third message");

    const auto firstIt = content.find("First message");
    const auto secondIt = content.find("Second message");
    const auto thirdIt = content.find("Third message");

    EXPECT_TRUE(firstPresent && secondPresent && thirdPresent);

    // Expect single thread consumer messages to be in order
    EXPECT_LT(firstIt, secondIt);
    EXPECT_LT(secondIt, thirdIt);
}

TEST_F(LoggerTest, EmptyMessage) {
    EXPECT_NO_THROW({
        m_Logger.LogInfo("");
        m_Logger.Flush();
    });
}

TEST_F(LoggerTest, LongMessage) {
    const std::string message(10'000, 'A');

    EXPECT_NO_THROW({
        m_Logger.LogInfo(message);
        m_Logger.Flush();
    });

    EXPECT_TRUE(ReadLogFile().contains(message));
}

TEST_F(LoggerTest, SpecialCharacters) {
    m_Logger.LogInfo("Hello\nWorld\t\"quoted\"\u2615");
    m_Logger.Flush();

    const auto content = ReadLogFile();

    EXPECT_TRUE(content.contains("Hello\n"));
    EXPECT_TRUE(content.contains("World\t"));
    EXPECT_TRUE(content.contains("\"quoted\""));
    EXPECT_TRUE(content.contains("\u2615"));
}
