#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <glm/glm.hpp>

#include "shader.h"

#include "raytracer_basics.h"

class TriangleShader : public Shader {
public:
    // A ray "ray" excites a point "is" in the scene "scene".
    // The function should compute the color of the point by emitting additional rays.
    // To emit the rays, the objects in the scene should be considered (another objects or light sources)
    // so this class dependes on the scene.
    TriangleShader(glm::vec3 color) : Shader(color) {}
    virtual glm::vec3 shade(std::weak_ptr<Scene> scene, const Ray& incRay, const Intersection& is) = 0;
};

class GenericTriangleShader : public TriangleShader {
public:
    GenericTriangleShader(glm::vec3 color) : TriangleShader(color) {};
    glm::vec3 shade(std::weak_ptr<Scene> scene, const Ray& incRay, const Intersection& is);
};

class Triangle : public Object{
public:
    std::shared_ptr<TriangleShader> shader;
    int material = 0;
    Triangle(glm::vec3 a_vertex0, glm::vec3 a_vertex1, glm::vec3 a_vertex2, const glm::vec3& color) : vertex0(a_vertex0), vertex1(a_vertex1), vertex2(a_vertex2), color(color){}
    void setShader(std::shared_ptr<TriangleShader> ts);
    glm::vec3 vertex0;
    glm::vec3 vertex1;
    glm::vec3 vertex2;
    bool intersect(const Ray& ray, glm::vec3& outIntersectionPoint, glm::vec3& n, glm::vec3& c);
    glm::vec3 excite0(std::shared_ptr<Scene> scene, const Ray& ray, Intersection& intersection);
    void transform(const glm::mat3x3& transformMatrix);
    glm::vec3 normal_;
    glm::vec3 normal();
    glm::vec3 color;
};

#endif