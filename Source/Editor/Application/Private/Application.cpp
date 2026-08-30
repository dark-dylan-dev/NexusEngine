// SPDX-License-Identifier: MIT

module NE.Editor.Application;

import NexusEngine;

namespace Nexus {

    void Application::run() {
        Logger logger;
        logger.LogInfo(std::format("Hello {}", Config::engineName));
        Window window(std::format("{} v{}", Config::engineName, Config::engineVersion));
        while (!window.ShouldClose()) {
            window.PollEvents();
            window.DrawFrame();
        }
    }

} // namespace Nexus
