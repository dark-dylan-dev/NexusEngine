# SPDX-License-Identifier: MIT

file(GLOB_RECURSE PRIVATE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/Private/*.cpp)
list(FILTER PRIVATE_SOURCES EXCLUDE REGEX ".*/Private/Backend/.*\\.cpp$")

file(GLOB_RECURSE PUBLIC_MODULES ${CMAKE_CURRENT_SOURCE_DIR}/Public/*.cppm)

file(GLOB_RECURSE PRIVATE_MODULES ${CMAKE_CURRENT_SOURCE_DIR}/Private/*.cppm)
list(FILTER PRIVATE_MODULES EXCLUDE REGEX ".*/Private/Backend/.*\\.cppm$")

if(NEXUS_RHI_VULKAN)
    target_compile_definitions(NexusEngine PRIVATE NEXUS_RHI_VULKAN)
    file(GLOB_RECURSE VULKAN_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/Private/Backend/Vulkan/*.cpp
    )
    file(GLOB_RECURSE VULKAN_MODULES
        ${CMAKE_CURRENT_SOURCE_DIR}/Private/Backend/Vulkan/*.cppm
    )
    list(APPEND PRIVATE_SOURCES ${VULKAN_SOURCES})
    list(APPEND PRIVATE_MODULES ${VULKAN_MODULES})
endif()

if(NEXUS_RHI_DX12)
    target_compile_definitions(NexusEngine PRIVATE NEXUS_RHI_DX12)
    file(GLOB_RECURSE DX12_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/Private/Backend/DX12/*.cpp
    )
    file(GLOB_RECURSE DX12_MODULES
        ${CMAKE_CURRENT_SOURCE_DIR}/Private/Backend/DX12/*.cppm
    )
    list(APPEND PRIVATE_SOURCES ${DX12_SOURCES})
    list(APPEND PRIVATE_MODULES ${DX12_MODULES})
endif()

target_sources(NexusEngine PRIVATE ${PRIVATE_SOURCES})
target_sources(NexusEngine PUBLIC
    FILE_SET engine_rhi_modules TYPE CXX_MODULES
    FILES ${PUBLIC_MODULES}
)
target_sources(NexusEngine PUBLIC
    FILE_SET engine_prv_rhi_modules TYPE CXX_MODULES
    FILES ${PRIVATE_MODULES}
)
