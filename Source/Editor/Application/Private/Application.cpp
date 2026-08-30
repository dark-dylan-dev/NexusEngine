// SPDX-License-Identifier: MIT

module NE.Editor.Application;

import NexusEngine;

namespace Nexus {

    void Application::run() {
        Logger logger;
        logger.LogInfo(std::format("Hello {}", Config::engineName));
        Window window("Nexus Engine");
        while (!window.ShouldClose()) {
            window.DrawFrame();
            window.PollEvents();
        }
    }

} // namespace Nexus
