#include <glad/glad.h>
#include <iostream>

class Framebuffer {
public:
    unsigned int framebuffer;
    unsigned int textureColorbuffer;
    unsigned int rbo;

    Framebuffer(unsigned int SCR_WIDTH, unsigned int SCR_HEIGHT);
    ~Framebuffer();

    void bind();
    void unbind();
};
