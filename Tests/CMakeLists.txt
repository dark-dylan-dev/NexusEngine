# SPDX-License-Identifier: MIT

add_executable(NexusTests main.cpp)

file(GLOB_RECURSE PRIVATE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE PRIVATE_MODULES ${CMAKE_CURRENT_SOURCE_DIR}/*.cppm)

target_sources(NexusTests PRIVATE ${PRIVATE_SOURCES})
target_sources(NexusTests PRIVATE
        FILE_SET tests_modules TYPE CXX_MODULES
        FILES ${PRIVATE_MODULES}
)

target_link_libraries(NexusTests PRIVATE NexusEngine GTest::gtest_main)
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_link_libraries(NexusTests PRIVATE "-lstdc++exp")
endif()

include(GoogleTest)
gtest_discover_tests(NexusTests)
