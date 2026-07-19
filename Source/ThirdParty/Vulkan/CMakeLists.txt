include(CMakeDependentOption)

block(PROPAGATE VK_VERSION_STRING)
    set(vulkan_core_header_file "${CMAKE_CURRENT_SOURCE_DIR}/vulkan/vulkan_core.h")
    if (NOT EXISTS ${vulkan_core_header_file})
        message(FATAL_ERROR "Couldn't find vulkan_core.h!")
    endif()

    file(READ ${vulkan_core_header_file} ver)

    if (ver MATCHES "#define[ ]+VK_HEADER_VERSION_COMPLETE[ ]+VK_MAKE_API_VERSION\\([ ]*[0-9]+,[ ]*([0-9]+),[ ]*([0-9]+),[ ]*VK_HEADER_VERSION[ ]*\\)")
        set(MAJOR_VERSION "${CMAKE_MATCH_1}")
        set(MINOR_VERSION "${CMAKE_MATCH_2}")
    else()
        message(FATAL_ERROR "Couldn't get major/minor version")
    endif()

    if (ver MATCHES "#define[ ]+VK_HEADER_VERSION[ ]+([0-9]+)")
        set(PATCH_VERSION "${CMAKE_MATCH_1}")
    else()
        message(FATAL_ERROR "Couldn't get the patch version")
    endif()

    set(VK_VERSION_STRING "${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION}")
endblock()

set(VULKAN_HEADERS_ENABLE_MODULE ON)

# set up Vulkan-Headers
add_library(Vulkan-Headers INTERFACE)
add_library(Vulkan::Headers ALIAS Vulkan-Headers)
target_compile_definitions(Vulkan-Headers INTERFACE VK_NO_PROTOTYPES=1)
target_include_directories(Vulkan-Headers INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

add_library(Vulkan-HppModule OBJECT)
target_compile_definitions(Vulkan-HppModule INTERFACE VULKAN_HPP_CXX_MODULE_EXPERIMENTAL_WARNING)
set_target_properties(Vulkan-HppModule PROPERTIES CXX_MODULE_STD ON)
add_library(Vulkan::HppModule ALIAS Vulkan-HppModule)
target_sources(Vulkan-HppModule PUBLIC
    FILE_SET CXX_MODULES
    BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}"
    FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/vulkan/vulkan.cppm"
        "${CMAKE_CURRENT_SOURCE_DIR}/vulkan/vulkan_video.cppm")

target_link_libraries(Vulkan-HppModule PUBLIC Vulkan::Headers)

target_include_directories(NexusEngine PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
message(STATUS "Vulkan: Version ${VK_VERSION_STRING}")