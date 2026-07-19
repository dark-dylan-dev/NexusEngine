add_subdirectory(Editor)
add_subdirectory(Engine)
add_subdirectory(ThirdParty)

target_link_libraries(NexusEngine PRIVATE NexusThirdParty)
target_link_libraries(NexusEditor PRIVATE NexusEngine)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_link_libraries(NexusEngine PRIVATE "-lstdc++exp")
    target_link_libraries(NexusEditor PRIVATE "-lstdc++exp")
endif()