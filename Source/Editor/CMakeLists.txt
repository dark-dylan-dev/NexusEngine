# SPDX-License-Identifier: MIT

add_executable(NexusEditor main.cpp)
set_target_properties(NexusEditor PROPERTIES OUTPUT_NAME "Nexus Engine")

add_subdirectory(Application)
add_subdirectory(Integrations)

target_sources(NexusEditor PUBLIC
        FILE_SET editor_module TYPE CXX_MODULES
        FILES NexusEditor.cppm
)

if(WIN32)
    target_sources(NexusEditor PRIVATE
        ${CMAKE_SOURCE_DIR}/Assets/Windows/NexusEditor.rc
    )
endif()

add_custom_command(
        TARGET NexusEditor POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${DISCORD_SHARED_LIB}"
            $<TARGET_FILE_DIR:NexusEditor>
)
