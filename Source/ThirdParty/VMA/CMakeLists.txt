add_library(VulkanMemoryAllocator INTERFACE)
add_library(GPUOpen::VulkanMemoryAllocator ALIAS VulkanMemoryAllocator)
add_library(VulkanMemoryAllocator::Headers ALIAS VulkanMemoryAllocator)

add_library(VulkanMemoryAllocator-Hpp INTERFACE)
add_library(VulkanMemoryAllocator-Hpp::VulkanMemoryAllocator-Hpp ALIAS VulkanMemoryAllocator-Hpp)
target_include_directories(VulkanMemoryAllocator-Hpp INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

if (TARGET Vulkan::HppModule)
    message(STATUS "VMA-Hpp module is available")

    # Set up VulkanMemoryAllocator-HppModule.
    add_library(VulkanMemoryAllocator-HppModule OBJECT)
    set_target_properties(VulkanMemoryAllocator-HppModule PROPERTIES CXX_MODULE_STD ON)
    add_library(VulkanMemoryAllocator-Hpp::VulkanMemoryAllocator-HppModule ALIAS VulkanMemoryAllocator-HppModule)
    target_sources(VulkanMemoryAllocator-HppModule PUBLIC
            FILE_SET CXX_MODULES
            FILES "${CMAKE_CURRENT_SOURCE_DIR}/vk_mem_alloc.cppm")
    target_link_libraries(VulkanMemoryAllocator-HppModule PUBLIC VulkanMemoryAllocator-Hpp Vulkan::HppModule)
endif ()

target_include_directories(NexusEngine PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
message(STATUS "VMA: Version 3.4.0")