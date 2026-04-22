#include "core/Window.h"
#include "renderer/Shader.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>

int main() {
    Window window(800, 600, "OpenGL Project");

    Shader shader("src/shaders/basic.vert", "src/shaders/basic.frag");

    float vertices[] = {
        0.5f,  0.25f,  0.0f, // top right
        0.5f,  -0.25f, 0.0f, // bottom right
        -0.5f, -0.25f, 0.0f, // bottom left
        -0.5f, 0.25f,  0.0f  // top left
    };
    unsigned int indices[] = {
        0, 1, 3, // first Triangle
        1, 2, 3  // second Triangle
    };

    shader.setupBuffers(vertices, sizeof(vertices), indices, sizeof(indices));

    while (!window.shouldClose()) {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader.use();
        shader.draw();

        window.swapBuffers();
        window.pollEvents();
    }

    return 0;
}
