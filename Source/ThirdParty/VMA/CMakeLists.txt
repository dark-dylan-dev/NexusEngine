if(NOT ${Vulkan_FOUND})
    message(FATAL_ERROR "Vulkan not present, can't include VMA")
    return()
endif()

add_library(VulkanMemoryAllocator INTERFACE)
add_library(GPUOpen::VulkanMemoryAllocator ALIAS VulkanMemoryAllocator)
add_library(VulkanMemoryAllocator::Headers ALIAS VulkanMemoryAllocator)

target_include_directories(VulkanMemoryAllocator INTERFACE ${Vulkan_INCLUDE_DIRS})

target_include_directories(NexusEngine PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
message(STATUS "VMA: Version 3.4.0")