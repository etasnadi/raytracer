#ifndef RAYTRACER_BASICS_H
#define RAYTRACER_BASICS_H

#include <memory>
#include <vector>

#include <glm/glm.hpp>

#include "cudastuff.h"

class Colors {
public:
    static glm::vec3 red;
    static glm::vec3 green;
    static glm::vec3 blue;
    static glm::vec3 yellow;
    static glm::vec3 magenta;
    static glm::vec3 cyan;
    static glm::vec3 black;
    static glm::vec3 gray;
    static glm::vec3 white;
};

class Object;
class Ray;

CUDA_HOSTDEV bool _closestIntersection(
    const Ray& ray,
    const std::vector<std::shared_ptr<Object>>& objects,
    glm::vec3& is,
    glm::vec3& n,
    glm::vec3& c,
    int& intersectedObjectId);

float sq(float x);
std::ostream& operator<< (std::ostream& os, Ray r);
std::ostream& operator<< (std::ostream& os, glm::vec3 o);
glm::vec3 normalize(const glm::vec3& inp);
glm::vec3 resize_unit(const glm::vec3& v);
template <typename T> int sgn(T val);

class Ray {
public:
    uint32_t bounces;
    glm::vec3 origin;
    glm::vec3 direction;
    CUDA_HOSTDEV Ray(glm::vec3 origin, glm::vec3 direction, uint32_t bounces = 5) : origin(origin), direction(direction), bounces(bounces) {};
};

typedef struct {
    glm::vec3 surfPt;
    glm::vec3 surfNormal;
    std::shared_ptr<Object> obj;
} Intersection;

class Light {
public:
    glm::vec3 lightPosition;
    glm::vec3 lightColor;
    Light(glm::vec3 lightPosition, glm::vec3 lightColor) : lightPosition(lightPosition), lightColor(lightColor) {};
};

class Scene : public std::enable_shared_from_this<Scene> {
public:
    std::vector<std::shared_ptr<Object>> objects;
    std::vector<std::shared_ptr<Light>> lights;

    void transform(glm::mat3x3 trans);
    CUDA_HOSTDEV bool closestIntersection(const Ray& ray, Intersection& result);
    CUDA_HOSTDEV bool trace(const Ray& ray, glm::vec3& result_color);
    glm::vec3 computeDiffuseComponent(const glm::vec3& surfPt, const glm::vec3 srufN, const glm::vec3& surfCol, bool shadows);
    virtual void buildScene() = 0;
};

class Object {
public:
    int material = 0;
    CUDA_HOSTDEV virtual bool intersect(const Ray& ray, glm::vec3& outIntersectionPoint, glm::vec3& outNormal, glm::vec3& outColor) = 0;
    virtual void transform(const glm::mat3x3& transformMatrix) = 0;
    CUDA_HOSTDEV virtual glm::vec3 excite0(std::shared_ptr<Scene> scene, const Ray& ray, Intersection& intersection) = 0;
};

#endif