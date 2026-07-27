# SPDX-License-Identifier: MIT

file(GLOB_RECURSE PRIVATE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/Private/*.cpp)
file(GLOB_RECURSE PUBLIC_MODULES ${CMAKE_CURRENT_SOURCE_DIR}/Public/*.cppm)
target_sources(NexusEditor PRIVATE ${PRIVATE_SOURCES})
target_sources(NexusEditor PUBLIC
        FILE_SET editor_application_modules TYPE CXX_MODULES
        FILES ${PUBLIC_MODULES}
)