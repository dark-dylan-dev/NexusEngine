module NE.Editor.Application;

import NexusEngine;
import std;

namespace Nexus {

    void Application::run() {
        std::println("Hello {}", Nexus::engineName);
    }

} // namespace Nexus