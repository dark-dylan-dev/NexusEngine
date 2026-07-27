# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.30 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_EXTENSIONS ON)

# C++20 modules
include(Scripts/CMake/SetModuleUUID.cmake)
setModuleID()

project(NexusEngine VERSION 0.0.1 LANGUAGES CXX)

add_subdirectory(Source)