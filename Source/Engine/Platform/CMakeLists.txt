# SPDX-License-Identifier: MIT

file(GLOB_RECURSE PRIVATE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

target_include_directories(NexusEngine PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Include)
target_sources(NexusEngine PRIVATE ${PRIVATE_SOURCES})
