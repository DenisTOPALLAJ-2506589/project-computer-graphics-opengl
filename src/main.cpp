#include "core/Window.h"
#include "renderer/Mesh.h"
#include "renderer/Shader.h"
#include "renderer/Texture.h"
#include "scene/Camera.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = 400.0f, lastY = 300.0f;
bool firstMouse = true;
float deltaTime = 0.0f, lastFrame = 0.0f;

void mouse_callback(GLFWwindow *window, double xposIn, double yposIn) {
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;
    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

int main() {
    Window window(800, 600, "OpenGL Project");
    glfwSetCursorPosCallback(window.getHandle(), mouse_callback);
    glfwSetScrollCallback(window.getHandle(), scroll_callback);
    glfwSetInputMode(window.getHandle(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glEnable(GL_DEPTH_TEST);

    Shader shader("src/shaders/basic.vert", "src/shaders/basic.frag");
    Texture texture("src/resources/train-carriage.png");

    std::vector<float> vertices = {
        // positions          // texture coords
        -1.0f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f,  -0.5f, -0.5f, 1.0f, 0.0f, 1.0f,  0.5f,  -0.5f, 1.0f, 1.0f,
        1.0f,  0.5f,  -0.5f, 1.0f, 1.0f, -1.0f, 0.5f,  -0.5f, 0.0f, 1.0f, -1.0f, -0.5f, -0.5f, 0.0f, 0.0f,

        -1.0f, -0.5f, 0.5f,  0.0f, 0.0f, 1.0f,  -0.5f, 0.5f,  1.0f, 0.0f, 1.0f,  0.5f,  0.5f,  1.0f, 1.0f,
        1.0f,  0.5f,  0.5f,  1.0f, 1.0f, -1.0f, 0.5f,  0.5f,  0.0f, 1.0f, -1.0f, -0.5f, 0.5f,  0.0f, 0.0f,

        -1.0f, 0.5f,  0.5f,  1.0f, 0.0f, -1.0f, 0.5f,  -0.5f, 1.0f, 1.0f, -1.0f, -0.5f, -0.5f, 0.0f, 1.0f,
        -1.0f, -0.5f, -0.5f, 0.0f, 1.0f, -1.0f, -0.5f, 0.5f,  0.0f, 0.0f, -1.0f, 0.5f,  0.5f,  1.0f, 0.0f,

        1.0f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f,  0.5f,  -0.5f, 1.0f, 1.0f, 1.0f,  -0.5f, -0.5f, 0.0f, 1.0f,
        1.0f,  -0.5f, -0.5f, 0.0f, 1.0f, 1.0f,  -0.5f, 0.5f,  0.0f, 0.0f, 1.0f,  0.5f,  0.5f,  1.0f, 0.0f,

        -1.0f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f,  -0.5f, -0.5f, 1.0f, 1.0f, 1.0f,  -0.5f, 0.5f,  1.0f, 0.0f,
        1.0f,  -0.5f, 0.5f,  1.0f, 0.0f, -1.0f, -0.5f, 0.5f,  0.0f, 0.0f, -1.0f, -0.5f, -0.5f, 0.0f, 1.0f,

        -1.0f, 0.5f,  -0.5f, 0.0f, 1.0f, 1.0f,  0.5f,  -0.5f, 1.0f, 1.0f, 1.0f,  0.5f,  0.5f,  1.0f, 0.0f,
        1.0f,  0.5f,  0.5f,  1.0f, 0.0f, -1.0f, 0.5f,  0.5f,  0.0f, 0.0f, -1.0f, 0.5f,  -0.5f, 0.0f, 1.0f};

    std::vector<unsigned int> indices;
    for (int i = 0; i < vertices.size() / 5; i++) {
        indices.push_back(i);
    }

    Mesh mesh(vertices, indices);

    while (!window.shouldClose()) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window.getHandle());

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom),
                                                (float)window.getWidth() / (float)window.getHeight(), 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);

        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        shader.setMat4("model", model);

        texture.bind();
        mesh.draw();

        window.swapBuffers();
        window.pollEvents();
    }

    return 0;
}
