// SPDX-License-Identifier: MIT

module;

#include <GLFW/glfw3.h>
#include <Platform/DLL/Export.hpp>

export module NE.Engine.Core.Window;

export namespace Nexus {
    class NEXUS_API Window {
    public:
        Window(const char* title);
        ~Window();

        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;

        bool ShouldClose() const;
        void DrawFrame();
        void PollEvents();

        int GetWidth() const;
        int GetHeight() const;

    private:
        void CreateWindow(const char* title);
        void CenterWindow();
        void SetIcon() const;

    private:
        GLFWwindow* m_window{nullptr};
        int m_width{};
        int m_height{};
    };
} // namespace Nexus
