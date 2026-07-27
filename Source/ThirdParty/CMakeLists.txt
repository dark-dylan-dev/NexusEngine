# SPDX-License-Identifier: MIT

add_library(NexusThirdParty INTERFACE)

add_subdirectory(GLFW)
add_subdirectory(GLM)
add_subdirectory(ImGui)
add_subdirectory(Jolt)
add_subdirectory(Volk)
add_subdirectory(Vulkan)
add_subdirectory(VMA)

target_link_libraries(NexusThirdParty INTERFACE glfw glm imgui Jolt volk VulkanMemoryAllocator-Hpp::VulkanMemoryAllocator-HppModule)