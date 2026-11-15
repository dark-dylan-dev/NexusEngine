# SPDX-License-Identifier: MIT

if(MSVC)
    set(NEXUS_COMPILE_FLAGS /arch:AVX2 /GL /Oi)
    set(NEXUS_LINK_FLAGS /LTCG)
else()
    set(NEXUS_COMPILE_FLAGS -march=native -mfma)
    set(NEXUS_LINK_FLAGS -flto)
endif()

add_compile_options(${NEXUS_COMPILE_FLAGS})
add_link_options(${NEXUS_LINK_FLAGS})

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
    target_link_libraries(NexusEngine PRIVATE stdc++exp)
    target_link_libraries(NexusEditor PRIVATE stdc++exp)
endif()
