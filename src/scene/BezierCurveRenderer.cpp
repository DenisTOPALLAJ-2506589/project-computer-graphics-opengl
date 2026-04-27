#include "BezierCurveRenderer.h"

BezierCurveRenderer::BezierCurveRenderer(const BezierCurve &curve, int samples) {
    std::vector<glm::vec3> points = curve.getSamplePoints(samples);
    vertexCount = static_cast<int>(points.size());

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(glm::vec3), points.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

BezierCurveRenderer::~BezierCurveRenderer() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void BezierCurveRenderer::draw(const Shader &shader) const {
    glBindVertexArray(VAO);
    glDrawArrays(GL_LINE_STRIP, 0, vertexCount);
    glBindVertexArray(0);
}
