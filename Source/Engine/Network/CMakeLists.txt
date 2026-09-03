# SPDX-License-Identifier: MIT

file(GLOB_RECURSE PRIVATE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*/Private/*.cpp)
file(GLOB_RECURSE PUBLIC_MODULES ${CMAKE_CURRENT_SOURCE_DIR}/*/Public/*.cppm)
file(GLOB_RECURSE PRIVATE_MODULES ${CMAKE_CURRENT_SOURCE_DIR}/*/Private/*.cppm)

if(WIN32)
    file(GLOB_RECURSE WINDOWS_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/Platform/Windows/*.cpp)
    list(APPEND PRIVATE_SOURCES ${WINDOWS_SOURCES})
elseif(LINUX)
    file(GLOB_RECURSE LINUX_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/Platform/Linux/*.cpp)
    list(APPEND PRIVATE_SOURCES ${LINUX_SOURCES})
endif()

target_sources(NexusEngine PRIVATE ${PRIVATE_SOURCES})
target_sources(NexusEngine PUBLIC
        FILE_SET engine_network_modules TYPE CXX_MODULES
        FILES ${PUBLIC_MODULES}
)
target_sources(NexusEngine PUBLIC
        FILE_SET engine_network_modules_prv TYPE CXX_MODULES
        FILES ${PRIVATE_MODULES}
)

if(WIN32)
    target_link_libraries(NexusEngine PRIVATE ws2_32)
endif()
