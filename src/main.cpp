#include "core/Window.h"
#include "renderer/Mesh.h"
#include "renderer/Shader.h"
#include "renderer/Texture.h"
#include "scene/BezierCurve.h"
#include "scene/BezierCurveRenderer.h"
#include "scene/Camera.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

Camera camera(glm::vec3(0.0f, 0.0f, 14.0f));
float lastX = 400.0f, lastY = 300.0f;
bool firstMouse = true;
float deltaTime = 0.0f, lastFrame = 0.0f;
float bezierDistance = 0.0f;
bool usingSecondCurve = false;
bool cKeyWasPressed = false;
const int numCarriages = 6;
const float carriageSpacing = 1.2f;

BezierCurve createFirstCurve() {
    // flat curve
    BezierCurve curve(glm::vec3(-6.0f, 0.0f, 0.0f), glm::vec3(-2.0f, 1.0f, 0.0f), glm::vec3(2.0f, -1.0f, 0.0f),
                      glm::vec3(6.0f, 0.0f, 0.0f));
    curve.buildLUT();
    return curve;
}

BezierCurve createSecondCurve() {
    // curve with height
    BezierCurve curve(glm::vec3(-6.0f, 0.0f, 0.0f), glm::vec3(-2.0f, 0.0f, 10.0f), glm::vec3(2.0f, 0.0f, -10.0f),
                      glm::vec3(6.0f, 0.0f, 0.0f));
    curve.buildLUT();
    return curve;
}

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
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);

    bool cKeyIsPressed = glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS;
    if (cKeyIsPressed && !cKeyWasPressed) {
        usingSecondCurve = !usingSecondCurve;
        bezierDistance = 0.0f; // reset position on curve switch
    }
    cKeyWasPressed = cKeyIsPressed;
}

void drawCarriages(Shader &shader, Mesh &mesh, BezierCurve &curve, int numCarriages, float spacing, bool flatCurve) {
    for (int i = 0; i < numCarriages; i++) {
        float d = fmod(bezierDistance + i * spacing, curve.getTotalLength());
        float t = curve.getTFromDistance(d);
        glm::vec3 pos = curve.evaluate(t);
        glm::vec3 tangent = glm::normalize(curve.evaluateTangent(t));

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, pos);

        if (flatCurve) {
            float angle = atan2(tangent.z, tangent.x);
            model = glm::rotate(model, angle, glm::vec3(0.0f, -1.0f, 0.0f));
        } else {
            float angle = atan2(tangent.y, tangent.x);
            model = glm::rotate(model, angle, glm::vec3(0.0f, 0.0f, 1.0f));
        }

        model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
        shader.setMat4("model", model);
        mesh.draw();
    }
}

void handleSceneUpdate(Window &window, BezierCurve &firstCurve, BezierCurve &secondCurve,
                       BezierCurveRenderer &firstRenderer, BezierCurveRenderer &secondRenderer,
                       BezierCurve *&activeCurve, BezierCurveRenderer *&activeRenderer) {
    processInput(window.getHandle());

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    activeCurve = usingSecondCurve ? &secondCurve : &firstCurve;
    activeRenderer = usingSecondCurve ? &secondRenderer : &firstRenderer;

    bezierDistance += deltaTime * 5.0f; // 5.0 units per second
    bezierDistance = fmod(bezierDistance, activeCurve->getTotalLength());
}

void renderBezierPath(Shader &lineShader, BezierCurveRenderer &activeRenderer, Window &window) {
    lineShader.use();
    lineShader.setMat4("projection",
                       glm::perspective(glm::radians(camera.Zoom), (float)window.getWidth() / (float)window.getHeight(),
                                        0.1f, 100.0f));
    lineShader.setMat4("view", camera.GetViewMatrix());
    lineShader.setMat4("model", glm::mat4(1.0f));
    lineShader.setVec3("color", glm::vec3(1.0f, 1.0f, 1.0f)); // White path
    activeRenderer.draw(lineShader);
}

int main() {
    Window window(800, 600, "OpenGL Project");
    glfwSetCursorPosCallback(window.getHandle(), mouse_callback);
    glfwSetScrollCallback(window.getHandle(), scroll_callback);
    glfwSetInputMode(window.getHandle(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glEnable(GL_DEPTH_TEST);

    Shader shader("src/shaders/basic.vert", "src/shaders/basic.frag");
    Shader lineShader("src/shaders/color.vert", "src/shaders/color.frag");
    Texture texture("src/resources/train-carriage.png");
    Shader lampShader("src/shaders/lamp.vert", "src/shaders/lamp.frag");

    glm::vec3 pointLightPositions[] = {
        glm::vec3(-3.0f, 4.0f, 0.0f),
        glm::vec3( 3.0f, 4.0f, 0.0f)
    };

    std::vector<float> vertices = {
        // positions          // normals           // texcoords
        // Back face
        -1.0f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
         1.0f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -1.0f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        -1.0f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

        // Front face
        -1.0f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
         1.0f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
         1.0f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
         1.0f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
        -1.0f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,

        // Left face
        -1.0f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -1.0f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -1.0f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -1.0f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -1.0f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -1.0f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        // Right face
         1.0f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
         1.0f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
         1.0f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         1.0f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         1.0f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
         1.0f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        // Bottom face
        -1.0f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
         1.0f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
         1.0f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
         1.0f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        -1.0f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
        -1.0f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

        // Top face
        -1.0f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
         1.0f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
         1.0f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
         1.0f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        -1.0f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
        -1.0f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };

    // Generate sequential indices (36 vertices, no sharing needed)
    std::vector<unsigned int> indices;
    for (unsigned int i = 0; i < 36; i++)
        indices.push_back(i);

    Mesh mesh(vertices, indices);

    // Lamp VAO — same VBO, just a different VAO
    unsigned int lampVAO;
    glGenVertexArrays(1, &lampVAO);
    glBindVertexArray(lampVAO);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.getVBO());
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    BezierCurve firstCurve = createFirstCurve();
    BezierCurve secondCurve = createSecondCurve();

    BezierCurveRenderer firstRenderer(firstCurve);
    BezierCurveRenderer secondRenderer(secondCurve);

    BezierCurve *activeCurve = nullptr;
    BezierCurveRenderer *activeRenderer = nullptr;

    while (!window.shouldClose()) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        handleSceneUpdate(window, firstCurve, secondCurve, firstRenderer, secondRenderer, activeCurve, activeRenderer);

        renderBezierPath(lineShader, *activeRenderer, window);

        shader.use();
        shader.setMat4("projection",
                   glm::perspective(glm::radians(camera.Zoom), (float)window.getWidth() / (float)window.getHeight(),
                        0.1f, 100.0f));
        shader.setMat4("view", camera.GetViewMatrix());

        shader.setVec3("viewPos", camera.Position);

        shader.setInt("material.diffuse",  0);
        shader.setInt("material.specular", 0);
        shader.setFloat("material.shininess", 32.0f);

        // LIGHTS
        // Point light 0 — left side, above the track
        shader.setVec3("pointLights[0].position",  glm::vec3(-3.0f, 4.0f, 0.0f));
        shader.setVec3("pointLights[0].ambient",   glm::vec3(0.1f, 0.1f, 0.1f));
        shader.setVec3("pointLights[0].diffuse",   glm::vec3(0.8f, 0.8f, 0.8f));
        shader.setVec3("pointLights[0].specular",  glm::vec3(1.0f, 1.0f, 1.0f));
        shader.setFloat("pointLights[0].constant",  1.0f);
        shader.setFloat("pointLights[0].linear",    0.09f);
        shader.setFloat("pointLights[0].quadratic", 0.032f);

        // Point light 1 — right side, above the track
        shader.setVec3("pointLights[1].position",  glm::vec3(3.0f, 4.0f, 0.0f));
        shader.setVec3("pointLights[1].ambient",   glm::vec3(0.1f, 0.1f, 0.1f));
        shader.setVec3("pointLights[1].diffuse",   glm::vec3(0.8f, 0.8f, 0.8f));
        shader.setVec3("pointLights[1].specular",  glm::vec3(1.0f, 1.0f, 1.0f));
        shader.setFloat("pointLights[1].constant",  1.0f);
        shader.setFloat("pointLights[1].linear",    0.09f);
        shader.setFloat("pointLights[1].quadratic", 0.032f);

        texture.bind();
        drawCarriages(shader, mesh, *activeCurve, numCarriages, carriageSpacing, !usingSecondCurve);

        lampShader.use();
        lampShader.setMat4("projection", glm::perspective(glm::radians(camera.Zoom),
            (float)window.getWidth() / (float)window.getHeight(), 0.1f, 100.0f));
        lampShader.setMat4("view", camera.GetViewMatrix());

        // Draw lamps at light positions
        glBindVertexArray(lampVAO);
        for (int i = 0; i < 2; i++) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.2f));
            lampShader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        glBindVertexArray(0);

        window.swapBuffers();
        window.pollEvents();
    }

    return 0;
}