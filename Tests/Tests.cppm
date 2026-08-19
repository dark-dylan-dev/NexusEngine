// SPDX-License-Identifier: MIT

module;

#include <gtest/gtest.h>

export module NexusTests;

TEST(TestExample, AdditionWorks) {
    EXPECT_EQ(2 + 2, 4);
}

export namespace Nexus::Tests {
    int RunTests(int argc, char* argv[]) {
        ::testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
    }
} // namespace Nexus::Tests
