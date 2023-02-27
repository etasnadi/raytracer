#ifndef MODELS_H
#define MODELS_H

#include <memory>
#include <vector>

#include <glm/glm.hpp>

#include "sphere.h"
#include "triangle.h"

class Models {
public:
  std::vector<std::shared_ptr<Triangle>>
  createBox(glm::vec3 center, float edgeSize,
            const std::vector<glm::vec3> &colors);
  std::vector<std::shared_ptr<Triangle>>
  createSquare(glm::vec3 topLeft, glm::vec3 topRight, glm::vec3 bottomRight,
               glm::vec3 bottomLeft, const glm::vec3 &color);
};

#endif