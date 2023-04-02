#include "raytracer_basics.h"

#include <iostream>

#include "basic_types.h"
#include "cudastuff.h"

namespace raytracer_cu {

CUDA_HOSTDEV bool _closestIntersection(Ray &ray, EasyVector<Object *> &objects,
                                       float3 &is, float3 &n, float3 &c,
                                       int &intersectedObjectId,
                                       bool shadowRay = false) {

  float minDist = 99999.0f;
  float3 minDistIs;
  float3 minDistN;
  float3 minDistC;
  int minDistObjectId = -1;

  for (int o_id = 0; o_id < objects.size(); o_id++) {
    Object *o = objects[o_id];
    float3 currIsPoint;
    float3 currIsN;
    float3 currIsC;

    bool ii = o->intersect(ray, currIsPoint, currIsN, currIsC);
    if (ii) {

      float3 dd(currIsPoint - ray.origin);
      float currDist = length(dd);
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

float3 Scene::computeDiffuseComponent(float3 &surfacePoint,
                                      float3 &surfaceNormal,
                                      float3 &surfaceColor, bool shadows) {
  float3 diffuseReflection = make_float3(0.0f, 0.0f, 0.0f);

  for (int lightId = 0; lightId < lights.size(); lightId++) {

    Light *light = lights[lightId];
    float3 surfacePointToLight = light->lightPosition - surfacePoint;
    float3 surfacePointToLightNormalized =
        div(surfacePointToLight, length(surfacePointToLight));
    float angle = dot(surfacePointToLightNormalized, surfaceNormal);

    // Generate shadow ray
    bool rayOccluded = false;
    if (shadows) {

      // float3 occludedObjSurfPt;
      // float3 unused;
      // int occludedObjectId;

      Ray shadowRay(surfacePoint, surfacePointToLight);

      Intersection closestObject;
      bool occlusion = closestIntersection(shadowRay, closestObject);

      float occludedObjDist = length(closestObject.surfacePoint - surfacePoint);

      float lightDist = length(surfacePointToLight);
      if (occlusion && occludedObjDist < lightDist) {
        rayOccluded = true;
      }
    }

    if (angle >= 0. && !rayOccluded) {
      diffuseReflection = diffuseReflection + surfaceColor * angle;
    }
  }
  return diffuseReflection;
}

bool Scene::closestIntersection(Ray &incidentRay,
                                Intersection &surfaceIntersection,
                                bool shadowRay) {

  float3 unused;

  int intersectedObjectId;
  bool hit = _closestIntersection(incidentRay, sceneObjects,
                                  surfaceIntersection.surfacePoint,
                                  surfaceIntersection.surfaceNormal, unused,
                                  intersectedObjectId, shadowRay);
  if (hit) {
    surfaceIntersection.object = sceneObjects[intersectedObjectId];
  }
  return hit;
}

bool Scene::trace(Ray &ray, float3 &emittedColor) {
  Intersection surfaceIntersection;

  bool hit = closestIntersection(ray, surfaceIntersection);
  if (hit) {
    emittedColor =
        surfaceIntersection.object->excite(this, ray, surfaceIntersection);
  }
  return hit;
}

void Scene::transform(mat3x3 trans) {
  for (int i = 0; i < sceneObjects.size(); i++) {
    Object *o = sceneObjects[i];
    o->transform(trans);
  }

  for (int i = 0; i < lights.size(); i++) {
    Light *l = lights[i];
    l->lightPosition = mm<3>(trans, l->lightPosition);
  }
}

// sgn function is missing from stdlib.
// https://stackoverflow.com/a/4609795
template <typename T> int sgn(T val) { return (T(0) < val) - (val < T(0)); }

// ---------- Ray functions ----------

std::ostream &operator<<(std::ostream &os, Ray r) {
  os << "[" << r.origin << " --> " << r.direction << "]";
  return os;
}

} // namespace raytracer_cu