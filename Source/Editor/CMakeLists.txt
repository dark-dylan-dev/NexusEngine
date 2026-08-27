# SPDX-License-Identifier: MIT

add_executable(NexusEditor main.cpp)

add_subdirectory(Application)

target_sources(NexusEditor PUBLIC
        FILE_SET editor_module TYPE CXX_MODULES
        FILES NexusEditor.cppm
)

if(WIN32 AND MSVC AND CMAKE_BUILD_TYPE MATCHES "^(Debug|RelWithDebInfo)$")
    add_custom_command(
        TARGET NexusEditor POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E rm -f
            "$<TARGET_FILE_DIR:NexusEditor>/NexusEditor.ilk"
    )
endif()
