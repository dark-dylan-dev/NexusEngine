# SPDX-License-Identifier: MIT

add_subdirectory(Editor)
add_subdirectory(Engine)
add_subdirectory(ThirdParty)

set_target_properties(NexusEngine NexusEditor PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

target_link_libraries(NexusEngine PRIVATE NexusThirdParty)
target_link_libraries(NexusEditor PRIVATE NexusEngine)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_link_libraries(NexusEngine PRIVATE "-lstdc++exp")
    target_link_libraries(NexusEditor PRIVATE "-lstdc++exp")
endif()
