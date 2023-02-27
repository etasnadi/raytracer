#include "models.h"

#include <memory>
#include <vector>

std::vector<std::shared_ptr<Triangle>>
Models::createSquare(glm::vec3 topLeft, glm::vec3 topRight,
                     glm::vec3 bottomRight, glm::vec3 bottomLeft,
                     const glm::vec3 &color) {
  auto t1 = std::make_shared<Triangle>(topLeft, topRight, bottomLeft, color);
  auto t2 =
      std::make_shared<Triangle>(bottomLeft, topRight, bottomRight, color);
  return {t1, t2};
}

std::vector<std::shared_ptr<Triangle>>
Models::createBox(glm::vec3 center, float edgeSize,
                  const std::vector<glm::vec3> &colors) {
  // Top vertices
  glm::vec3 topTopLeft = center + glm::vec3(-edgeSize, -edgeSize, edgeSize);
  glm::vec3 topTopRight = center + glm::vec3(edgeSize, -edgeSize, edgeSize);
  glm::vec3 topBottomRight = center + glm::vec3(edgeSize, -edgeSize, -edgeSize);
  glm::vec3 topBottomLeft = center + glm::vec3(-edgeSize, -edgeSize, -edgeSize);

  // Bottom vertices
  glm::vec3 bottomTopLeft = center + glm::vec3(-edgeSize, edgeSize, edgeSize);
  glm::vec3 bottomTopRight = center + glm::vec3(edgeSize, edgeSize, edgeSize);
  glm::vec3 bottomBottomRight =
      center + glm::vec3(edgeSize, edgeSize, -edgeSize);
  glm::vec3 bottomBottomLeft =
      center + glm::vec3(-edgeSize, edgeSize, -edgeSize);

  // Sides
  auto top = createSquare(topTopLeft, topTopRight, topBottomRight,
                          topBottomLeft, colors[0]); // Green
  auto bottom = createSquare(bottomTopLeft, bottomBottomLeft, bottomBottomRight,
                             bottomTopRight, colors[1]); // Red

  auto left = createSquare(topTopLeft, topBottomLeft, bottomBottomLeft,
                           bottomTopLeft, colors[2]); // Blue +
  auto right = createSquare(topTopRight, bottomTopRight, bottomBottomRight,
                            topBottomRight, colors[3]); // Yellow

  auto back = createSquare(topTopLeft, bottomTopLeft, bottomTopRight,
                           topTopRight, colors[4]); // Magenta +
  auto front = createSquare(topBottomLeft, topBottomRight, bottomBottomRight,
                            bottomBottomLeft, colors[5]); // Cyan +

  return {top[0],   top[1],   bottom[0], bottom[1], left[0],  left[1],
          right[0], right[1], back[0],   back[1],   front[0], front[1]};
}
