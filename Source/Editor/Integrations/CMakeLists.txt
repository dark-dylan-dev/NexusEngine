# SPDX-License-Identifier: MIT

add_subdirectory(Discord)

target_sources(NexusEditor PUBLIC
        FILE_SET editor_integrations_module TYPE CXX_MODULES
        FILES "${CMAKE_CURRENT_SOURCE_DIR}/Integrations.cppm"
)
