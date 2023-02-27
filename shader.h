#ifndef SHADER_H
#define SHADER_H

#include <memory>

#include <glm/glm.hpp>

#include "raytracer_basics.h"

class Shader {
public:
  glm::vec3 color;
  float diffuseWeight = .5f;
  float reflectedWeight = .5f;
  float refractedWeight = 0.f;
  bool enableShadows = true;
  float refractiveIndex = 1.0f;
  float bounceSurfDist = .05f;
  float weightThreshold = 0.001;

  Shader() {}
  Shader(glm::vec3 color) : color(color) {}

  void setProfile(float a_diffuseWeight, float a_reflectedWeight,
                  float a_refractedWeight) {
    diffuseWeight = a_diffuseWeight;
    reflectedWeight = a_reflectedWeight;
    refractedWeight = a_refractedWeight;
  }
  virtual bool computeReflectiveComponent(
      std::shared_ptr<Scene const> scene, const Ray &incidentRay,
      const Intersection &surfaceIntersection, glm::vec3 &reflectedColor) const;
  virtual bool
  computeRefractiveComponent(std::shared_ptr<Scene const> scene,
                             const Ray &incidentRay,
                             const Intersection &surfaceIntersection,
                             float refractiveIndex, glm::vec3 &refractedColor) const;
};

#endif