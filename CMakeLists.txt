# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.30 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_EXTENSIONS ON)

# C++20 modules
include(Scripts/CMake/SetModuleUUID.cmake)
setModuleID()

# Default UTF-8 on Windows
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

# Default debug build
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

project(NexusEngine VERSION 0.0.1 LANGUAGES CXX)

add_subdirectory(Source)

option(NEXUS_BUILD_TEST "Enable Nexus Engine tests" OFF)
if(NEXUS_BUILD_TEST)
    include(Scripts/CMake/EnableTesting.cmake)
    enable_testing()
    add_subdirectory(Tests)
endif()
