add_library(VulkanMemoryAllocator INTERFACE)
add_library(GPUOpen::VulkanMemoryAllocator ALIAS VulkanMemoryAllocator)
add_library(VulkanMemoryAllocator::Headers ALIAS VulkanMemoryAllocator)

target_include_directories(VulkanMemoryAllocator INTERFACE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>")

# VMA Configuration Flags
target_compile_definitions(VulkanMemoryAllocator INTERFACE VMA_STATIC_VULKAN_FUNCTIONS=0)
target_compile_definitions(VulkanMemoryAllocator INTERFACE VMA_DYNAMIC_VULKAN_FUNCTIONS=1)
target_sources(VulkanMemoryAllocator INTERFACE VMA.cpp)

add_library(VulkanMemoryAllocator-Hpp INTERFACE)
target_compile_options(VulkanMemoryAllocator-Hpp INTERFACE
        $<$<CXX_COMPILER_ID:Clang>:-Wno-nullability-completeness>
)
add_library(VulkanMemoryAllocator-Hpp::VulkanMemoryAllocator-Hpp ALIAS VulkanMemoryAllocator-Hpp)
add_library(VulkanMemoryAllocator::Hpp ALIAS VulkanMemoryAllocator-Hpp)
target_include_directories(VulkanMemoryAllocator-Hpp INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

if (TARGET Vulkan-HppModule)
    message(STATUS "VMA-Hpp module is available")

    # Set up VulkanMemoryAllocator-HppModule.
    add_library(VulkanMemoryAllocator-HppModule)
    set_target_properties(VulkanMemoryAllocator-HppModule PROPERTIES CXX_MODULE_STD ON)
    add_library(VulkanMemoryAllocator-Hpp::VulkanMemoryAllocator-HppModule ALIAS VulkanMemoryAllocator-HppModule)
    target_sources(VulkanMemoryAllocator-HppModule PUBLIC
            FILE_SET CXX_MODULES
            FILES "${CMAKE_CURRENT_SOURCE_DIR}/vk_mem_alloc.cppm")
    target_link_libraries(VulkanMemoryAllocator-HppModule PUBLIC VulkanMemoryAllocator-Hpp Vulkan-HppModule)

    # fallback
    if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/vk_mem_alloc.h")
        target_link_libraries(VulkanMemoryAllocator-HppModule PUBLIC GPUOpen::VulkanMemoryAllocator)
    else ()
        if (NOT TARGET GPUOpen::VulkanMemoryAllocator)
            find_package(VulkanMemoryAllocator CONFIG REQUIRED)
        endif ()
        target_link_libraries(VulkanMemoryAllocator-HppModule PUBLIC GPUOpen::VulkanMemoryAllocator)
    endif ()
endif ()

target_include_directories(NexusEngine PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# VMA version
set(VMA_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/vk_mem_alloc.h")
file(READ "${VMA_HEADER}" VMA_FILE_CONTENT)
string(REGEX MATCH "#define[ \t]+VMA_VERSION[ \t]+\\(?VK_MAKE_VERSION\\(([0-9]+),[ \t]*([0-9]+),[ \t]*([0-9]+)\\)\\)?"
       _ ${VMA_FILE_CONTENT})

set(VMA_VERSION_MAJOR "${CMAKE_MATCH_1}")
set(VMA_VERSION_MINOR "${CMAKE_MATCH_2}")
set(VMA_VERSION_PATCH "${CMAKE_MATCH_3}")

set(VMA_VERSION_STRING "${VMA_VERSION_MAJOR}.${VMA_VERSION_MINOR}.${VMA_VERSION_PATCH}")
message(STATUS "VMA: Version ${VMA_VERSION_STRING}")
