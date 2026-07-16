set(IMGUI_SOURCES
        imgui.h
        imgui_internal.h
        imgui.cpp
        imgui_draw.cpp
        imgui_tables.cpp
        imgui_widgets.cpp
        imgui_demo.cpp
)
add_library(imgui STATIC ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(imgui PRIVATE glfw Vulkan::Vulkan)

message(STATUS "ImGui: Version 1.92.8 docking")