// SPDX-License-Identifier: MIT

module NE.Editor.Application;

import NexusEngine;

namespace Nexus {

    void Application::run() {
        GetEngineTime();

        Logger logger;
        logger.LogInfo(std::format("Hello {}", Config::engineName));
        Window window(std::format("{} v{}", Config::engineName, Config::engineVersion));
        auto m_CurrentWidth = window.GetWidth();
        auto m_CurrentHeight = window.GetHeight();
        const auto device = RHI::Device::Create();
        const auto surface = device->CreateSurface(window);

        while (!window.ShouldClose()) {
            window.PollEvents();
            int32 newWidth = window.GetWidth();
            int32 newHeight = window.GetHeight();

            if (newWidth != m_CurrentWidth || newHeight != m_CurrentHeight) {
                m_CurrentWidth = newWidth;
                m_CurrentHeight = newHeight;

                if (newWidth > 0 && newHeight > 0) {
                    surface->Resize(newWidth, newHeight);
                }
            }

            int32 pixelWidth = window.GetPixelWidth();
            int32 pixelHeight = window.GetPixelHeight();

            // Skip rendering while minimized
            if (pixelWidth == 0 || pixelHeight == 0) {
                std::this_thread::sleep_for(std::chrono::microseconds(100));
                continue;
            }
            surface->BeginFrame();
            surface->EndFrame();
        }
        logger.LogInfo(std::format("Engine ran for {:.3f} seconds", GetEngineTime()));
    }

} // namespace Nexus
