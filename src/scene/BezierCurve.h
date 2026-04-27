#include <glm/glm.hpp>
#include <vector>

class BezierCurve {
  public:
    BezierCurve(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3);
    glm::vec3 evaluate(float t) const;
    glm::vec3 evaluateTangent(float t) const;
    std::vector<glm::vec3> getSamplePoints(int numSamples = 100) const;
    float advanceT(float t, float deltaTime, float speed = 0.2f) const;

    glm::vec3 p0, p1, p2, p3;
};
