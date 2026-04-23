#include <glad/glad.h>
#include <vector>

class Mesh {
  public:
    Mesh(const std::vector<float> &vertices, const std::vector<unsigned int> &indices);
    ~Mesh();

    void draw() const;

  private:
    unsigned int VAO = 0, VBO = 0, EBO = 0;
    unsigned int indexCount = 0;
};