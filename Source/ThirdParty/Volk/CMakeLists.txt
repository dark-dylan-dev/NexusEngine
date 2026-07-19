add_library(volk STATIC volk.cpp)
target_include_directories(volk PRIVATE ${CMAKE_SOURCE_DIR}/Source/ThirdParty/Vulkan)

target_include_directories(NexusEngine PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
message(STATUS "Volk: Version 345")