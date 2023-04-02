#include "shader.h"

#include <iostream>
#include <memory>

#include "raytracer_basics.h"

namespace raytracer_cu {

CUDA_HOSTDEV float3 computeRafractionDirection(Ray &incidentRay,
                                               float3 &surfaceNormal,
                                               float &kn) {
  float3 projectedVector =
      div(incidentRay.direction,
          abs(dot(incidentRay.direction, surfaceNormal))); // V' in the paper
  float kf = 1.0f / sqrt(sq(kn) * sq(length(projectedVector)) -
                         sq(length(projectedVector + surfaceNormal)));
  return kf * (surfaceNormal + projectedVector) -
         surfaceNormal; // P in the paper
}

CUDA_HOSTDEV float3 computeReflectionDirection(Ray &incidentRay,
                                               float3 &surfaceNormal) {
  float3 projectedVector =
      div(incidentRay.direction,
          abs(dot(incidentRay.direction, surfaceNormal))); // V' in the paper
  return projectedVector + 2.0f * surfaceNormal;           // R in the paper
}

bool Shader::computeRefractiveComponent(Scene *scene, Ray &incidentRay,
                                        Intersection &surfaceIntersection,
                                        float refractiveIndex,
                                        float3 &refractedColor) {
  float adjustNormSign = 1.0f;
  if (dot(surfaceIntersection.surfaceNormal, incidentRay.direction) > -0.001f) {
    adjustNormSign = -1.0f;
  }

  float3 adjustedNormal = adjustNormSign * surfaceIntersection.surfaceNormal;
  float3 refrDir =
      computeRafractionDirection(incidentRay, adjustedNormal, refractiveIndex);
  float3 fixedSurfPt =
      surfaceIntersection.surfacePoint - bounceSurfDist * adjustedNormal;
  Ray refractionRay(fixedSurfPt, refrDir, incidentRay.bounces - 1);
  float3 tmpRefractedColor;
  bool result = scene->trace(refractionRay, tmpRefractedColor);

  float debugEps = .0001f;
  if (abs(incidentRay.direction.x) < debugEps &&
      abs(incidentRay.direction.y) < debugEps) {
#ifdef DEBUG_STDOUT
    std::cout << "Normal: " << adjustedNormal << "Incoming: " << incidentRay
              << " refracted: " << refractionRay
              << " color: " << tmpRefractedColor << " [" << incidentRay.bounces
              << "]" << std::endl;
#endif
  }

  refractedColor = tmpRefractedColor;
  return result;
}

bool Shader::computeReflectiveComponent(Scene *scene, Ray &incidentRay,
                                        Intersection &surfaceIntersection,
                                        float3 &reflectedColor) {
  float3 fixedSurfPt = surfaceIntersection.surfacePoint +
                       bounceSurfDist * surfaceIntersection.surfaceNormal;
  float3 refDir = computeReflectionDirection(incidentRay,
                                             surfaceIntersection.surfaceNormal);

  Ray reflectionRay(fixedSurfPt, refDir, incidentRay.bounces - 1);
  float3 reflectedColorTmp;

  bool result = scene->trace(reflectionRay, reflectedColorTmp);
  reflectedColor = reflectedColorTmp;
  return result;
}
} // namespace raytracer_cu
