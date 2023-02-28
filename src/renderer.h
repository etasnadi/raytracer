#ifndef RENDERER_H
#define RENDERER_H

#include <memory>

#include <glm/glm.hpp>

#include "basic_types.h"
#include "raytracer_basics.h"

namespace raytracer {
  class Renderer {
  private:
    float rx = 0.0f;
    float ry = 0.0f;
    float sensitivity = 0.1f;
    void applyInputTransforms();
    glm::ivec2 displaySize;
    glm::vec3 viewport_tl, viewport_v1, viewport_v2;
    glm::vec3 eye;


  public:
    std::shared_ptr<Scene> scene;
    std::vector<std::shared_ptr<ColorBuffer<glm::vec3>>> textures;
    Renderer(uint32_t screen_width, uint32_t screen_height);
    ColorBuffer<glm::vec3> render();
    void rotateLeft();
    void rotateRight();
    void rotateUp();
    void rotateDown();
    void registerTexture(ColorBuffer<glm::vec3>& texture);
  };
}

#endif