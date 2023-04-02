#ifndef SHADER_H
#define SHADER_H

#include <memory>

#include "raytracer_basics.h"

namespace raytracer_cu {

class Shader {
public:
  float3 color;
  float diffuseWeight = 0.5f;
  float reflectedWeight = 0.5f;
  float refractedWeight = 0.0f;
  bool enableShadows = true;
  float refractiveIndex = 1.0f;
  float bounceSurfDist = 0.05f;
  float weightThreshold = 0.001f;

  CUDA_HOSTDEV Shader() {}
  CUDA_HOSTDEV Shader(float3 color) : color(color) {}

  CUDA_HOSTDEV void setProfile(float a_diffuseWeight, float a_reflectedWeight,
                               float a_refractedWeight) {
    diffuseWeight = a_diffuseWeight;
    reflectedWeight = a_reflectedWeight;
    refractedWeight = a_refractedWeight;
  }
  CUDA_HOSTDEV virtual bool
  computeReflectiveComponent(Scene *scene, Ray &incidentRay,
                             Intersection &surfaceIntersection,
                             float3 &reflectedColor);
  CUDA_HOSTDEV virtual bool
  computeRefractiveComponent(Scene *scene, Ray &incidentRay,
                             Intersection &surfaceIntersection,
                             float refractiveIndex, float3 &refractedColor);
};
} // namespace raytracer_cu

#endif