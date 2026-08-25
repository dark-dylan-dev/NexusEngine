// SPDX-License-Identifier: MIT

#pragma once

#if defined(_WIN32) && defined(_MSC_VER)
#    ifdef NEXUS_ENGINE_BUILD
#        define NEXUS_API __declspec(dllexport)
#    else
#        define NEXUS_API __declspec(dllimport)
#    endif
#else
#    define NEXUS_API __attribute__((visibility("default")))
#endif
