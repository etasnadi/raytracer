#ifndef MODELS_H
#define MODELS_H

#include <vector>
#include <memory>

#include <glm/glm.hpp>

#include "triangle.h"
#include "sphere.h"

class Models {
public:
    std::vector<std::shared_ptr<Triangle>> createBox(glm::vec3 center, float w);
    std::vector<std::shared_ptr<Triangle>> square(glm::vec3 tl, glm::vec3 tr, glm::vec3 br, glm::vec3 bl, const glm::vec3& color);
};

#endif