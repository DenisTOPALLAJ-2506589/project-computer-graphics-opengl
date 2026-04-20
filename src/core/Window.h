#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>

class Window {
  public:
    Window(int width, int height, const std::string &title);
    ~Window();

    bool shouldClose() const;
    void swapBuffers() const;
    void pollEvents() const;

    GLFWwindow *getHandle() const { return m_handle; }
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }

  private:
    GLFWwindow *m_handle = nullptr;
    int m_width, m_height;

    static void framebufferSizeCallback(GLFWwindow *window, int width, int height);
};
