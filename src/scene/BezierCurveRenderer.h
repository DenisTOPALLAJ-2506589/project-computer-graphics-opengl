#ifndef BEZIER_CURVE_RENDERER_H
#define BEZIER_CURVE_RENDERER_H

#include "BezierCurve.h"
#include "../renderer/Shader.h"
#include <glad/glad.h>
#include <vector>

class BezierCurveRenderer {
  public:
    BezierCurveRenderer(const BezierCurve &curve, int samples = 100);
    ~BezierCurveRenderer();

    void draw(const Shader &shader) const;

  private:
    unsigned int VAO, VBO;
    int vertexCount;
};

#endif
