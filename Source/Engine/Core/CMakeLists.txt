# SPDX-License-Identifier: MIT

file(GLOB_RECURSE PRIVATE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*/Private/*.cpp)
file(GLOB_RECURSE PUBLIC_MODULES ${CMAKE_CURRENT_SOURCE_DIR}/*/Public/*.cppm)

target_include_directories(NexusEngine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Config/Public)

target_sources(NexusEngine PRIVATE ${PRIVATE_SOURCES})
target_sources(NexusEngine PUBLIC
        FILE_SET engine_core_modules TYPE CXX_MODULES
        FILES ${PUBLIC_MODULES}
)
