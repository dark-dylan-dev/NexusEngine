// SPDX-License-Identifier: MIT

module;

#include <GLFW/glfw3.h>
#include <Platform/DLL/Export.hpp>

export module NE.Engine.Core.Window;

import std;

export namespace Nexus {
    class NEXUS_API Window {
    public:
        Window(const std::string_view& title);
        ~Window();

        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;

        bool ShouldClose() const;
        void DrawFrame();
        void PollEvents() const;

        int GetWidth() const;
        int GetHeight() const;

    private:
        void CreateWindow(const std::string_view& title);
        void CenterWindow() const;
        void SetIcon() const;

    private:
        GLFWwindow* m_window{nullptr};
        int m_width{};
        int m_height{};
    };
} // namespace Nexus
