// SPDX-License-Identifier: MIT

module;

#include <GLFW/glfw3.h>
#include <stb_image.h>

module NE.Engine.Core.Window;

import std;

namespace Nexus {
    Window::Window(const char* title) {
        if (!glfwInit())
            return;

        CreateWindow(title);

        if (!m_window) {
            glfwTerminate();
            return;
        }

        CenterWindow();
        SetIcon();
    }

    Window::~Window() {
        if (m_window) {
            glfwDestroyWindow(m_window);
            m_window = nullptr;
        }

        glfwTerminate();
    }

    bool Window::ShouldClose() const {
        return glfwWindowShouldClose(m_window);
    }

    void Window::DrawFrame() {}

    void Window::PollEvents() {
        glfwPollEvents();
    }

    int Window::GetWidth() const {
        return m_width;
    }

    int Window::GetHeight() const {
        return m_height;
    }

    void Window::CreateWindow(const char* title) {
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        m_width = mode->width / 2;
        m_height = mode->height / 2;

        m_window = glfwCreateWindow(m_width, m_height, title, nullptr, nullptr);
    }

    void Window::CenterWindow() {
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        const int xpos = mode->width / 2 - m_width / 2;
        const int ypos = mode->height / 2 - m_height / 2;
        glfwSetWindowPos(m_window, xpos, ypos);
    }

    void Window::SetIcon() const {
        constexpr std::array sizes{16, 32, 48};
        std::array<GLFWimage, sizes.size()> icons{};

        for (size_t i = 0; i < sizes.size(); ++i) {
            const auto path = std::format("Assets/NexusEditor@{}.png", sizes[i]);
            icons[i].pixels = stbi_load(path.c_str(), &icons[i].width, &icons[i].height, nullptr, STBI_rgb_alpha);
        }

        glfwSetWindowIcon(m_window, icons.size(), icons.data());

        for (const auto& icon : icons)
            stbi_image_free(icon.pixels);
    }
} // namespace Nexus
