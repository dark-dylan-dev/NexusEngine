// SPDX-License-Identifier: MIT

module NE.Editor.Application;

import NexusEngine;

namespace Nexus {

    void Application::run() {
        Logger logger;
        logger.Log(LogLevel::Info, std::format("Hello n°1 {}", Config::engineName));
        std::system("pause");
    }

} // namespace Nexus
