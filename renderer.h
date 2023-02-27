#ifndef RENDERER_H
#define RENDERER_H

#include <glm/glm.hpp>

#include "raytracer_basics.h"


template<class T>
class ColorBuffer {
private:
    std::vector<T> c;
public:
    const int w, h;
    ColorBuffer(int w, int h);
    void setPixel(int x, int y, T intensity);
    T getPixel(int x, int y);
    int idx(int x, int y);
};

class Renderer{
private:
    std::shared_ptr<Scene> scene;
    float rx = 0.f;
    float ry = 0.f;
    float sensitivity = .1f;
    void applyInputTransforms();
    glm::ivec2 displaySize;
    glm::vec3 viewport_tl, viewport_v1, viewport_v2;
    glm::vec3 eye;

public:
    Renderer(uint32_t screen_width, uint32_t screen_height);
    ColorBuffer<glm::vec3> render();
    void rotateLeft();
    void rotateRight();
    void rotateUp();
    void rotateDown();
};

#endif