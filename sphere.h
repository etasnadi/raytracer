#ifndef SPHERE_H
#define SPHERE_H

#include <memory>

#include <glm/glm.hpp>

#include "raytracer_basics.h"
#include "shader.h"

class SphereShader : public Shader {
public:
    // A ray "ray" excites a point "is" in the scene "scene".
    // The function should compute the color of the point by emitting additional rays.
    // To emit the rays, the objects in the scene should be considered (another objects or light sources)
    // so this class dependes on the scene.
    SphereShader(glm::vec3 color) : Shader(color) {};
    virtual glm::vec3 shade(std::weak_ptr<Scene> scene, const Ray& incRay, const Intersection& is) = 0;
};

class GenericSphereShader : public SphereShader {
public:
    GenericSphereShader(glm::vec3 color) : SphereShader(color) {};
    glm::vec3 shade(std::weak_ptr<Scene> scene, const Ray& incRay, const Intersection& is);
};

class Sphere : public Object{
public:
    std::shared_ptr<SphereShader> shader;
    int material = 0;
    glm::vec3 center;
    float r;
    glm::vec3 color;
    Sphere(){};
    Sphere(glm::vec3 center, float r, glm::vec3 color) : center(center), r(r), color(color){};
    glm::vec3 excite0(std::shared_ptr<Scene> scene, const Ray& ray, Intersection& intersection);
    void setShader(std::shared_ptr<SphereShader> ts);
    bool intersect(const Ray& ray, glm::vec3& outIntersectionPoint, glm::vec3& n, glm::vec3& c);
    void transform(const glm::mat3x3& transformMatrix);
};

#endif