#include "raytracer_basics.h"

#include <iostream>

#include <glm/glm.hpp>

#include "cudastuff.h"

CUDA_HOSTDEV bool _closestIntersection(
    const Ray &ray, const std::vector<std::shared_ptr<Object>> &objects,
    glm::vec3 &is, glm::vec3 &n, glm::vec3 &c, int &intersectedObjectId) {

  float minDist = std::numeric_limits<float>::infinity();
  glm::vec3 minDistIs;
  glm::vec3 minDistN;
  glm::vec3 minDistC;
  int minDistObjectId = -1;

  for (int o_id = 0; o_id < objects.size(); o_id++) {
    std::shared_ptr<Object> o = objects[o_id];
    glm::vec3 currIsPoint;
    glm::vec3 currIsN;
    glm::vec3 currIsC;
    if (o->intersect(ray, currIsPoint, currIsN, currIsC)) {
      glm::vec3 dd(currIsPoint - ray.origin);
      float currDist = glm::length(dd);
      if (currDist < minDist) {
        minDist = currDist;
        minDistIs = currIsPoint;
        minDistC = currIsC;
        minDistN = currIsN;
        minDistObjectId = o_id;
      }
    }
  }
  if (minDistObjectId >= 0) {
    is = minDistIs;
    n = minDistN;
    c = minDistC;
    intersectedObjectId = minDistObjectId;

    return true;
  } else {
    return false;
  }
}

// ---------- Scene functions ----------

glm::vec3 Scene::computeDiffuseComponent(const glm::vec3 &surfacePoint,
                                         const glm::vec3 surfaceNormal,
                                         const glm::vec3 &surfaceColor,
                                         bool shadows) const {
  glm::vec3 diffuseReflection = glm::vec3(0.0f, 0.0f, 0.0f);
  for (int lightId = 0; lightId < lights.size(); lightId++) {
    std::shared_ptr<Light> light = lights[lightId];
    glm::vec3 surfacePointToLight = light->lightPosition - surfacePoint;
    glm::vec3 surfacePointToLightNormalized =
        surfacePointToLight / glm::length(surfacePointToLight);
    float angle = glm::dot(surfacePointToLightNormalized, surfaceNormal);

    // Generate shadow ray
    bool rayOccluded = false;
    if (shadows) {
      glm::vec3 occludedObjSurfPt;
      glm::vec3 unused;
      int occludedObjectId;
      Ray shadowRay(surfacePoint, surfacePointToLight);
      Intersection closestObject;
      bool occlusion = closestIntersection(shadowRay, closestObject);

      float occludedObjDist =
          glm::length(closestObject.surfacePoint - surfacePoint);

      float lightDist = glm::length(surfacePointToLight);
      if (occlusion && occludedObjDist < lightDist) {
        rayOccluded = true;
      }
    }

    if (angle >= 0. && !rayOccluded) {
      diffuseReflection += surfaceColor * angle;
    }
  }
  return diffuseReflection;
}

CUDA_HOSTDEV bool
Scene::closestIntersection(const Ray &incidentRay,
                           Intersection &surfaceIntersection) const {
  glm::vec3 unused;

  int intersectedObjectId;
  bool hit = _closestIntersection(
      incidentRay, sceneObjects, surfaceIntersection.surfacePoint,
      surfaceIntersection.surfaceNormal, unused, intersectedObjectId);
  if (hit) {
    surfaceIntersection.object = sceneObjects[intersectedObjectId];
  }
  return hit;
}

CUDA_HOSTDEV bool Scene::trace(const Ray &ray, glm::vec3 &emittedColor) const {
  Intersection surfaceIntersection;

  bool hit = closestIntersection(ray, surfaceIntersection);
  if (hit) {
    emittedColor = surfaceIntersection.object->excite(shared_from_this(), ray,
                                                      surfaceIntersection);
  }
  return hit;
}

void Scene::transform(glm::mat3x3 trans) {
  for (std::shared_ptr<Object> o : sceneObjects) {
    o->transform(trans);
  }

  for (std::shared_ptr<Light> l : lights) {
    l->lightPosition = trans * (l->lightPosition);
  }
}

// sgn function is missing from stdlib.
// https://stackoverflow.com/a/4609795
template <typename T> int sgn(T val) { return (T(0) < val) - (val < T(0)); }

// ---------- vec3 utility functions ----------

glm::vec3 normalize(const glm::vec3 &inp) { return inp / glm::length(inp); }

std::ostream &operator<<(std::ostream &os, glm::vec3 o) {
  os << "(" << o.x << "," << o.y << "," << o.z << ")";
  return os;
}

// ---------- Ray functions ----------

std::ostream &operator<<(std::ostream &os, Ray r) {
  os << "[" << r.origin << " --> " << r.direction << "]";
  return os;
}

// ---------- basic algorithms ----------

// It can be faster than pow(,2) in several circumstances.
// https://stackoverflow.com/a/2940800
float square(float x) { return x * x; }