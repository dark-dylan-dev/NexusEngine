add_library(NexusEngine)

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