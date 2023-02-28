#ifndef SPHERE_H
#define SPHERE_H

#include <memory>

#include <glm/glm.hpp>

#include "raytracer_basics.h"
#include "shader.h"
namespace raytracer {
  class SphereShader : public Shader {
  public:
    // A ray "ray" excites a point "is" in the scene "scene".
    // The function should compute the color of the point by emitting additional
    // rays. To emit the rays, the objects in the scene should be considered
    // (another objects or light sources) so this class dependes on the scene.
    SphereShader(glm::vec3 color) : Shader(color){};
    virtual glm::vec3 shade(std::weak_ptr<Scene const> scene, const Ray &incidentRay,
                            const Intersection &intersection) const = 0;
  };

  class GenericSphereShader : public SphereShader {
  public:
    GenericSphereShader(glm::vec3 color) : SphereShader(color){};
    glm::vec3 shade(std::weak_ptr<Scene const> scene, const Ray &incidentRay,
                    const Intersection &intersection) const;
  };

  class Sphere : public Object {
  public:
    std::shared_ptr<SphereShader> shader;
    glm::vec3 center;
    float r;

    Sphere(glm::vec3 center, float r, glm::vec3 color)
        : center(center), r(r), Object(color){};
    glm::vec3 excite(std::shared_ptr<Scene const> scene, const Ray &incidentRay,
                    Intersection &intersection) const;
    void setShader(std::shared_ptr<SphereShader> sphereShader);
    bool intersect(const Ray &ray, glm::vec3 &outIntersectionPoint, glm::vec3 &n,
                  glm::vec3 &c);
    void transform(const glm::mat3x3 &transformMatrix);
  };
}

#endif
