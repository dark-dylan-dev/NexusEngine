// SPDX-License-Identifier: MIT

module NE.Editor.Application;

import NexusEngine;

namespace Nexus {

    void Application::run() {
        GetEngineTime();

        Logger logger;
        logger.LogInfo(std::format("Hello {}", Config::engineName));
        const auto title = std::format("{} v{}", Config::engineName, Config::engineVersion);
        Window window(title);
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

            static uint32 frameCount = 0;
            static float64 lastFpsUpdate = 0.0;
            static float64 fps = 0.0;

            const float64 t = GetEngineTime();

            ++frameCount;

            if (const float64 elapsed = t - lastFpsUpdate; elapsed >= 0.25) {
                fps = static_cast<float64>(frameCount) / elapsed;

                frameCount = 0;
                lastFpsUpdate = t;

                window.SetTitle(std::format("{} - FPS: {:.1f}", title, fps));
            }

            int32 pixelWidth = window.GetPixelWidth();
            int32 pixelHeight = window.GetPixelHeight();

            // Skip rendering while minimized
            if (pixelWidth == 0 || pixelHeight == 0) {
                std::this_thread::sleep_for(std::chrono::microseconds(100));
                continue;
            }
            surface->BeginFrame();
            auto wave = [](const float64 x) { return 0.5 + 0.5 * std::sin(x); };

            surface->ClearScreen(wave(t * 0.4) * 0.7 + wave(t * 0.13 + 1.0) * 0.3,
                                 wave(t * 0.3 + 2.0) * 0.7 + wave(t * 0.17) * 0.3,
                                 wave(t * 0.5 + 4.0) * 0.7 + wave(t * 0.11 + 2.0) * 0.3);
            surface->EndFrame();
        }
        logger.LogInfo(std::format("Engine ran for {:.3f} seconds", GetEngineTime()));
    }

} // namespace Nexus
