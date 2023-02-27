#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <glm/glm.hpp>

#include "raytracer_basics.h"
#include "shader.h"

class TriangleShader : public Shader {
public:
  // A ray "ray" excites a point "is" in the scene "scene".
  // The function should compute the color of the point by emitting additional
  // rays. To emit the rays, the objects in the scene should be considered
  // (another objects or light sources) so this class dependes on the scene.
  TriangleShader(glm::vec3 color) : Shader(color) {}
  virtual glm::vec3 shade(std::weak_ptr<Scene const> scene,
                          const Ray &incidentRay,
                          const Intersection &intersection) const = 0;
};

class GenericTriangleShader : public TriangleShader {
public:
  GenericTriangleShader(glm::vec3 color) : TriangleShader(color){};
  glm::vec3 shade(std::weak_ptr<Scene const> scene, const Ray &incidentRay,
                  const Intersection &intersection) const;
};

class Triangle : public Object {
public:
  std::shared_ptr<TriangleShader> shader;
  glm::vec3 vertex0;
  glm::vec3 vertex1;
  glm::vec3 vertex2;
  // The normals can be cached
  glm::vec3 normal_;

  Triangle(const glm::vec3 vertex0, glm::vec3 vertex1, glm::vec3 vertex2,
           glm::vec3 color)
      : vertex0(vertex0), vertex1(vertex1), vertex2(vertex2), Object(color) {}
  void setShader(std::shared_ptr<TriangleShader> triangleShader);
  glm::vec3 excite(std::shared_ptr<Scene const> scene, const Ray &incidentRay,
                   Intersection &intersection) const;
  bool intersect(const Ray &incidentRay, glm::vec3 &intersectionPoint,
                 glm::vec3 &surfaceNormal, glm::vec3 &surfaceColor);
  void transform(const glm::mat3x3 &transformMatrix);
  glm::vec3 normal();
};

#endif