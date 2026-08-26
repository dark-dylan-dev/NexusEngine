# SPDX-License-Identifier: MIT

add_library(NexusEngine SHARED)
set_target_properties(NexusEngine PROPERTIES PREFIX "")

target_compile_definitions(NexusEngine PRIVATE NEXUS_ENGINE_BUILD)
set_target_properties(NexusEngine PROPERTIES
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON
)
target_compile_definitions(NexusEngine PRIVATE
        $<$<CONFIG:Debug>:APP_BUILD_MODE=0>
        $<$<CONFIG:RelWithDebInfo>:APP_BUILD_MODE=1>
        $<$<CONFIG:Release>:APP_BUILD_MODE=2>
        $<$<CONFIG:MinSizeRel>:APP_BUILD_MODE=2>
)

add_subdirectory(Asset)
add_subdirectory(Audio)
add_subdirectory(Core)
add_subdirectory(Input)
add_subdirectory(Physics)
add_subdirectory(Platform)
add_subdirectory(Renderer)
add_subdirectory(RHI)
add_subdirectory(Scripting)

target_sources(NexusEngine PUBLIC
        FILE_SET engine_module TYPE CXX_MODULES
        FILES NexusEngine.cppm
)

if(WIN32)
    add_custom_command(
            TARGET NexusEngine POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "$<TARGET_FILE:NexusEngine>"
            "$<TARGET_FILE_DIR:NexusEditor>"
    )
    if(MSVC AND $<$<NOT:$<OR:$<CONFIG:Release>,$<CONFIG:MinSizeRel>>>>)
        add_custom_command(
                TARGET NexusEngine POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "$<TARGET_PDB_FILE:NexusEngine>"
                "$<TARGET_FILE_DIR:NexusEditor>/NexusEngine.pdb"
        )
    endif()
endif()
