file(GLOB_RECURSE IMGUI_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
file(GLOB_RECURSE IMGUI_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_library(imgui STATIC ${IMGUI_HEADERS} ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(imgui PRIVATE glfw Vulkan::Headers)

message(STATUS "ImGui: Version 1.92.8 docking")