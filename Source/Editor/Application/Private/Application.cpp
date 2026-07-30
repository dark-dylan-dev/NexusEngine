// SPDX-License-Identifier: MIT

module NE.Editor.Application;

import NexusEngine;

import std;

namespace Nexus {

    void Application::run() {
        std::println("Hello {}", Config::engineName);
    }

} // namespace Nexus
