# SPDX-License-Identifier: MIT

set(DISCORD_SDK_VERSION "1.10.19337")

set(DISCORD_SDK_LIB_DIR     "${CMAKE_CURRENT_SOURCE_DIR}/lib")
set(DISCORD_SDK_BIN_DIR     "${CMAKE_CURRENT_SOURCE_DIR}/bin")
set(DISCORD_SDK_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

add_library(DiscordPartnerSDK SHARED IMPORTED GLOBAL)

set_target_properties(DiscordPartnerSDK PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${DISCORD_SDK_INCLUDE_DIR}"
)

if(WIN32)
    set_target_properties(DiscordPartnerSDK PROPERTIES
            IMPORTED_IMPLIB   "${DISCORD_SDK_LIB_DIR}/discord_partner_sdk.lib"
            IMPORTED_LOCATION "${DISCORD_SDK_BIN_DIR}/discord_partner_sdk.dll"
    )
elseif(APPLE)
    set_target_properties(DiscordPartnerSDK PROPERTIES
            IMPORTED_LOCATION "${DISCORD_SDK_LIB_DIR}/libdiscord_partner_sdk.dylib"
    )
else() # Linux
    set_target_properties(DiscordPartnerSDK PROPERTIES
            IMPORTED_LOCATION "${DISCORD_SDK_LIB_DIR}/libdiscord_partner_sdk.so"
    )
endif()

message(STATUS "DiscordSocialSDK: Version ${DISCORD_SDK_VERSION}")
