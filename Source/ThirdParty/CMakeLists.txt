add_library(NexusThirdParty STATIC compile.cpp)

find_package(Vulkan 1.4 REQUIRED)

add_subdirectory(GLFW)
add_subdirectory(GLM)
add_subdirectory(ImGui)
add_subdirectory(Jolt)
add_subdirectory(VMA)
add_subdirectory(Vulkan)

target_link_libraries(NexusThirdParty PUBLIC glfw glm imgui Jolt GPUOpen::VulkanMemoryAllocator Vulkan::Vulkan)