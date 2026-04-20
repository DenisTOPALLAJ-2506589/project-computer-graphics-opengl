#include "core/Window.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>

int main() {
    Window window(800, 600, "OpenGL Project");
    while (!window.shouldClose()) {

        window.swapBuffers();
        window.pollEvents();
    }
    return 0;
}
