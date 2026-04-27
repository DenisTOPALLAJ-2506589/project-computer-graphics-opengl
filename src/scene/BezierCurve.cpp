#include "BezierCurve.h"
#include <math.h>

/* SOURCES
 * https://www.inf.ufsc.br/~aldo.vw/grafica/apostilas/openGL/lesson29/lesson29.html
 * https://medium.com/@PradumnaVerma/mastering-bezier-curves-unlocking-smoothness-in-computer-graphics-with-opengl-cd20e91557e5
 */

BezierCurve::BezierCurve(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) : p0(p0), p1(p1), p2(p2), p3(p3) {}

glm::vec3 BezierCurve::evaluate(float t) const {
    float u = 1.0f - t;
    return pow(u, 3.0f) * p0 + (3.0f * pow(u, 2.0f) * t) * p1 + (3.0f * u * pow(t, 2.0f)) * p2 + pow(t, 3.0f) * p3;
}

glm::vec3 BezierCurve::evaluateTangent(float t) const {
    float u = 1.0f - t;
    return 3.0f * pow(u, 2.0f) * (p1 - p0) + 6.0f * u * t * (p2 - p1) + 3.0f * pow(t, 2.0f) * (p3 - p2);
}

std::vector<glm::vec3> BezierCurve::getSamplePoints(int numSamples) const {
    std::vector<glm::vec3> points;
    points.reserve(numSamples + 1);

    for (int i = 0; i <= numSamples; i++) {
        float t = static_cast<float>(i) / static_cast<float>(numSamples);
        points.push_back(evaluate(t));
    }

    return points;
}

void BezierCurve::buildLUT(int numSamples) {
    lut.clear();
    totalLength = 0.0f;
    lut.push_back({0.0f, 0.0f});

    glm::vec3 prevPoint = p0;
    for (int i = 1; i <= numSamples; i++) {
        float t = static_cast<float>(i) / static_cast<float>(numSamples);
        glm::vec3 currPoint = evaluate(t);
        totalLength += glm::distance(currPoint, prevPoint);
        lut.push_back({t, totalLength});
        prevPoint = currPoint;
    }
}

float BezierCurve::getTFromDistance(float distance) const {
    if (lut.empty())
        return 0.0f;
    if (distance <= 0.0f)
        return lut.front().t;
    if (distance >= totalLength)
        return lut.back().t;

    for (size_t i = 0; i < lut.size() - 1; i++) {
        if (distance <= lut[i + 1].distance) {
            float d0 = lut[i].distance;
            float d1 = lut[i + 1].distance;
            float t0 = lut[i].t;
            float t1 = lut[i + 1].t;

            float f = (distance - d0) / (d1 - d0);
            return t0 + f * (t1 - t0);
        }
    }
    return lut.back().t;
}
