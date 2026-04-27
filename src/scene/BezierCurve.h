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

    BezierCurve(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3);
    glm::vec3 evaluate(float t) const;
    glm::vec3 evaluateTangent(float t) const;
    std::vector<glm::vec3> getSamplePoints(int numSamples = 100) const;
    
    // Arc Length Parameterization
    void buildLUT(int numSamples = 200);
    float getTFromDistance(float distance) const;
    float getTotalLength() const { return totalLength; }

    glm::vec3 p0, p1, p2, p3;

  private:
    std::vector<ArcLengthSample> lut;
    float totalLength = 0.0f;
};

#endif
