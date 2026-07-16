add_executable(NexusEditor main.cpp)

add_subdirectory(Application)
add_subdirectory(Panels)

target_sources(NexusEditor PUBLIC
        FILE_SET editor_module TYPE CXX_MODULES
        FILES NexusEditor.cppm
)
