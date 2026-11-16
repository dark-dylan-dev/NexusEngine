# SPDX-License-Identifier: MIT

file(GLOB_RECURSE PRIVATE_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/Private/*.cpp)
file(GLOB_RECURSE PUBLIC_MODULES  CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/Public/*.cppm)

target_link_libraries(NexusEditor PRIVATE DiscordPartnerSDK)

target_sources(NexusEditor PRIVATE ${PRIVATE_SOURCES})
target_sources(NexusEditor PUBLIC
        FILE_SET editor_discord_modules TYPE CXX_MODULES
        BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/Public
        FILES ${PUBLIC_MODULES}
)

if(UNIX)
    if(APPLE)
        set(_discord_rpath "@executable_path")
    else()
        set(_discord_rpath "$ORIGIN")
    endif()

    set_property(TARGET NexusEditor APPEND PROPERTY BUILD_RPATH   "${_discord_rpath}")
    set_property(TARGET NexusEditor APPEND PROPERTY INSTALL_RPATH "${_discord_rpath}")
endif()
