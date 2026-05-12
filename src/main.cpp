#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <vector>

#include "core/Window.h"
#include "renderer/Mesh.h"
#include "renderer/Shader.h"
#include "renderer/Texture.h"
#include "scene/BezierCurve.h"
#include "scene/BezierCurveRenderer.h"
#include "scene/Camera.h"

Camera camera(glm::vec3(0.0f, 0.0f, 14.0f));
float lastX = 400.0f, lastY = 300.0f;
bool firstMouse = true;
float deltaTime = 0.0f, lastFrame = 0.0f;
float bezierDistance = 0.0f;
bool usingSecondCurve = false;
bool cKeyWasPressed = false;
const int numCarriages = 6;
const float carriageSpacing = 1.2f;

bool cameraMode = false;
bool bKeyWasPressed = false;
bool fpvActive = false;
bool fKeyWasPressed = false;

glm::vec3 buttonWorldPos = glm::vec3(0.0f, 3.0f, 0.0f);
float buttonClickRadius = 25.0f;
bool buttonWasPressed = false;

std::vector<glm::vec3> generateCircularLoopPoints(float r, float h) {
    /* SOURCES
     * https://www.geeksforgeeks.org/cpp/bezier-curves-in-opengl/
     * https://stackoverflow.com/questions/1734745/how-to-create-circle-with-b%C3%A9zier-curves (magic number)
     */
    float k = 0.552284749831f * r; // magic number for circular bezier curves => 4*(sqrt(2)-1)/3
    return {
        {r, 0.0f, 0.0f}, {r, h, k},    {k, -h, r},       {0.0f, 0.0f, r}, // Segment 1
        {-k, h, r},      {-r, -h, k},  {-r, 0.0f, 0.0f},                  // Segment 2
        {-r, h, -k},     {-k, -h, -r}, {0.0f, 0.0f, -r},                  // Segment 3
        {k, h, -r},      {r, -h, -k},  {r, 0.0f, 0.0f}                    // Segment 4
    };
}

BezierCurve createFirstLoopCurve() {
    BezierCurve curve(generateCircularLoopPoints(10.0f, 2.0f)); // radius 10, height 2
    curve.buildLUT(400);
    return curve;
}

BezierCurve createSecondLoopCurve() {
    BezierCurve curve(generateCircularLoopPoints(7.0f, 5.0f)); // radius 7, height 5
    curve.buildLUT(400);
    return curve;
}

unsigned int setupLampVAO(Mesh &mesh) {
    unsigned int lampVAO;
    glGenVertexArrays(1, &lampVAO);
    glBindVertexArray(lampVAO);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.getVBO());
    // Only need positions for the lamps
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
    return lampVAO;
}

void configureLighting(Shader &shader, glm::vec3 *lightPositions, glm::vec3 cameraPos) {
    shader.use();
    shader.setVec3("viewPos", cameraPos);
    shader.setInt("material.diffuse", 0);
    shader.setInt("material.specular", 0);
    shader.setFloat("material.shininess", 32.0f);

    for (int i = 0; i < 8; i++) {
        std::string prefix = "pointLights[" + std::to_string(i) + "].";
        shader.setVec3(prefix + "position", lightPositions[i]);
        shader.setVec3(prefix + "ambient", glm::vec3(0.1f, 0.1f, 0.1f));
        shader.setVec3(prefix + "diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
        shader.setVec3(prefix + "specular", glm::vec3(1.0f, 1.0f, 1.0f));
        shader.setFloat(prefix + "constant", 1.0f);
        shader.setFloat(prefix + "linear", 0.09f);
        shader.setFloat(prefix + "quadratic", 0.032f);
    }
}

void renderLamps(Shader &lampShader, unsigned int vao, glm::vec3 *lightPositions, const glm::mat4 &view,
                 const glm::mat4 &projection) {
    lampShader.use();
    lampShader.setMat4("view", view);
    lampShader.setMat4("projection", projection);

    glBindVertexArray(vao);
    for (int i = 0; i < 8; i++) {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, lightPositions[i]);
        model = glm::scale(model, glm::vec3(0.2f));
        lampShader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    glBindVertexArray(0);
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

    if (cameraMode || glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        camera.ProcessMouseMovement(xoffset, yoffset);
    }
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

void checkButtonInteraction(GLFWwindow *window, Window &win) {
    bool leftMousePressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    if (leftMousePressed && !buttonWasPressed) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        glm::mat4 projection =
            glm::perspective(glm::radians(camera.Zoom), (float)win.getWidth() / (float)win.getHeight(), 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::vec4 viewport(0, 0, win.getWidth(), win.getHeight());
        glm::vec3 screenPos = glm::project(buttonWorldPos, view, projection, viewport);
        float screenY = win.getHeight() - screenPos.y;
        float dist = glm::distance(glm::vec2(xpos, ypos), glm::vec2(screenPos.x, screenY));
        if (dist < buttonClickRadius) {
            usingSecondCurve = !usingSecondCurve;
            bezierDistance = 0.0f;
        }
    }
    buttonWasPressed = leftMousePressed;
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS && !fpvActive)
        camera.ProcessKeyboard(DOWN, deltaTime);

    if (!fpvActive) {
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
    }

    bool cKeyIsPressed = glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS;
    if (cKeyIsPressed && !cKeyWasPressed) {
        usingSecondCurve = !usingSecondCurve;
        bezierDistance = 0.0f; // reset position on curve switch
    }
    cKeyWasPressed = cKeyIsPressed;

    bool bKeyIsPressed = glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS;
    if (bKeyIsPressed && !bKeyWasPressed) {
        cameraMode = !cameraMode;
        if (cameraMode || fpvActive)
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        else
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
    bKeyWasPressed = bKeyIsPressed;

    bool fKeyIsPressed = glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS;
    if (fKeyIsPressed && !fKeyWasPressed) {
        fpvActive = !fpvActive;
        if (fpvActive)
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        else if (!cameraMode)
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
    fKeyWasPressed = fKeyIsPressed;
}

void drawCarriages(Shader &shader, Mesh &mesh, BezierCurve &curve, int numCarriages, float spacing, bool flatCurve) {
    shader.use();
    for (int i = 0; i < numCarriages; i++) {
        float d = fmod(bezierDistance + i * spacing, curve.getTotalLength());
        float t = curve.getTFromDistance(d);
        glm::vec3 pos = curve.evaluate(t);
        glm::vec3 tangent = glm::normalize(curve.evaluateTangent(t));

        // middle cart
        if (fpvActive && i == numCarriages / 2) {
            camera.Position = pos + glm::vec3(0.0f, 1.0f, 0.0f); // height offset
        }

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, pos);

        // calc rotation to face tangent
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 right = glm::normalize(glm::cross(tangent, up));
        up = glm::normalize(glm::cross(right, tangent));

        glm::mat4 rotation = glm::mat4(1.0f);
        rotation[0] = glm::vec4(tangent, 0.0f);
        rotation[1] = glm::vec4(up, 0.0f);
        rotation[2] = glm::vec4(right, 0.0f);
        model = model * rotation;

        model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
        shader.setMat4("model", model);
        mesh.draw();
    }
}

void handleSceneUpdate(Window &window, BezierCurve &firstCurve, BezierCurve &secondCurve,
                       BezierCurveRenderer &firstRenderer, BezierCurveRenderer &secondRenderer,
                       BezierCurve *&activeCurve, BezierCurveRenderer *&activeRenderer) {
    processInput(window.getHandle());
    checkButtonInteraction(window.getHandle(), window);

    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
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

void drawButton(Shader &lineShader, Mesh &mesh, Window &window) {
    lineShader.use();
    lineShader.setMat4("projection",
                       glm::perspective(glm::radians(camera.Zoom), (float)window.getWidth() / (float)window.getHeight(),
                                        0.1f, 100.0f));
    lineShader.setMat4("view", camera.GetViewMatrix());

    glm::mat4 buttonModel = glm::mat4(1.0f);
    buttonModel = glm::translate(buttonModel, buttonWorldPos);
    buttonModel = glm::scale(buttonModel, glm::vec3(0.4f, 0.4f, 0.4f));
    lineShader.setMat4("model", buttonModel);
    lineShader.setVec3("color", glm::vec3(1.0f, 0.5f, 0.0f)); // Orange

    mesh.draw();
}

int main() {
    Window window(800, 600, "Rollercoaster Scene");
    glfwSetCursorPosCallback(window.getHandle(), mouse_callback);
    glfwSetScrollCallback(window.getHandle(), scroll_callback);
    glfwSetInputMode(window.getHandle(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    glEnable(GL_DEPTH_TEST);

    Shader shader("src/shaders/basic.vert", "src/shaders/basic.frag");
    Shader lineShader("src/shaders/color.vert", "src/shaders/color.frag");
    Shader lampShader("src/shaders/lamp.vert", "src/shaders/lamp.frag");
    Texture texture("src/resources/train-carriage.png");

    glm::vec3 pointLightPositions[] = {
        glm::vec3(-10.0f, 1.0f, 0.0f), glm::vec3(10.0f, 1.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, -10.0f), glm::vec3(0.0f, 1.0f, 10.0f),
        glm::vec3(-10.0f, 1.0f, -10.0f), glm::vec3(10.0f, 1.0f, -10.0f),
        glm::vec3(-10.0f, 1.0f, 10.0f), glm::vec3(10.0f, 1.0f, 10.0f)
    };

    std::vector<float> vertices = {
        -1.0f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f, 0.0f, 1.0f,  -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 1.0f, 0.0f,
        1.0f,  0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f, 1.0f, 1.0f, 1.0f,  0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f, 1.0f, 1.0f,
        -1.0f, 0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f, 1.0f, -1.0f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f, 0.0f,

        -1.0f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f,  -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
        1.0f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f, 1.0f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
        -1.0f, 0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f, -1.0f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,

        -1.0f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,  1.0f, 0.0f, -1.0f, 0.5f,  -0.5f, -1.0f, 0.0f,  0.0f,  1.0f, 1.0f,
        -1.0f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f,  0.0f, 1.0f, -1.0f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f,  0.0f, 1.0f,
        -1.0f, -0.5f, 0.5f,  -1.0f, 0.0f,  0.0f,  0.0f, 0.0f, -1.0f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,  1.0f, 0.0f,

        1.0f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f,  0.5f,  -0.5f, 1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        1.0f,  -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,  0.0f, 1.0f, 1.0f,  -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        1.0f,  -0.5f, 0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f, 1.0f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        -1.0f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  0.0f, 1.0f, 1.0f,  -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  1.0f, 1.0f,
        1.0f,  -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,  1.0f, 0.0f, 1.0f,  -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,  1.0f, 0.0f,
        -1.0f, -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,  0.0f, 0.0f, -1.0f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  0.0f, 1.0f,

        -1.0f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f,  0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
        1.0f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f, 1.0f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        -1.0f, 0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f, -1.0f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  0.0f, 1.0f};

    // Generate sequential indices (36 vertices, no sharing needed)
    std::vector<unsigned int> indices;
    for (int i = 0; i < vertices.size() / 8; i++)
        indices.push_back(i);

    Mesh mesh(vertices, indices);
    unsigned int lampVAO = setupLampVAO(mesh);

    // Lamp VAO — same VBO, just a different VAO
    glGenVertexArrays(1, &lampVAO);
    glBindVertexArray(lampVAO);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.getVBO());
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    BezierCurve firstCurve = createFirstLoopCurve();
    BezierCurve secondCurve = createSecondLoopCurve();

    BezierCurveRenderer firstRenderer(firstCurve, 400);
    BezierCurveRenderer secondRenderer(secondCurve, 400);

    BezierCurve *activeCurve = nullptr;
    BezierCurveRenderer *activeRenderer = nullptr;

    while (!window.shouldClose()) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        handleSceneUpdate(window, firstCurve, secondCurve, firstRenderer, secondRenderer, activeCurve, activeRenderer);

        renderBezierPath(lineShader, *activeRenderer, window);
        drawButton(lineShader, mesh, window);

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom),
                                                (float)window.getWidth() / (float)window.getHeight(), 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();

        configureLighting(shader, pointLightPositions, camera.Position);
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);

        texture.bind();
        drawCarriages(shader, mesh, *activeCurve, numCarriages, carriageSpacing, usingSecondCurve);

        renderLamps(lampShader, lampVAO, pointLightPositions, view, projection);

        window.swapBuffers();
        window.pollEvents();
    }

    return 0;
}
