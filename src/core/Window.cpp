#include "Window.h"
#include <iostream>

Window::Window(int width, int height, const std::string &title) : m_width(width), m_height(height) {
    if (!glfwInit()) {
        std::cerr << "Failed initalization of GLFW\n";
        return;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_handle = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!m_handle) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(m_handle);
    glfwSetFramebufferSizeCallback(m_handle, framebufferSizeCallback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        glfwTerminate();
        m_handle = nullptr;
    }
}

Window::~Window() { glfwTerminate(); }
bool Window::shouldClose() const { return m_handle ? glfwWindowShouldClose(m_handle) : true; }

void Window::swapBuffers() const {
    if (m_handle)
        glfwSwapBuffers(m_handle);
}

void Window::pollEvents() const { glfwPollEvents(); }

void Window::framebufferSizeCallback(GLFWwindow * /*window*/, int width, int height) {
    glViewport(0, 0, width, height);
}
