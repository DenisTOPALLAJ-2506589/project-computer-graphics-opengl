#pragma once

#include <glad/glad.h>
#include <string>

class Shader {
  public:
    unsigned int ID;
    unsigned int VAO, VBO, EBO;
    unsigned int indexCount;

    Shader(const char* vertexPath, const char* fragmentPath);
    ~Shader();

    void setupBuffers(float* vertices, size_t verticesSize, unsigned int* indices, size_t indicesSize);
    void draw();

    void use();
    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
};
