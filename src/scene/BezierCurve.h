#ifndef BEZIER_CURVE_H
#define BEZIER_CURVE_H

#include <glm/glm.hpp>
#include <vector>

class BezierCurve {
  public:
    struct ArcLengthSample {
        float t;
        float distance;
    };

    BezierCurve(const std::vector<glm::vec3> &points);
    glm::vec3 evaluate(float t) const;
    glm::vec3 evaluateTangent(float t) const;
    std::vector<glm::vec3> getSamplePoints(int numSamples = 100) const;

    void buildLUT(int numSamples = 200);
    float getTFromDistance(float distance) const;
    float getTotalLength() const { return totalLength; }

    std::vector<glm::vec3> controlPoints;

  private:
    std::vector<ArcLengthSample> lut;
    float totalLength = 0.0f;
};

#endif
