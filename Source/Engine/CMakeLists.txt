# SPDX-License-Identifier: MIT

add_library(NexusEngine)

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
