#ifndef SPHERE_H
#define SPHERE_H

#include <memory>

#include "raytracer_basics.h"
#include "shader.h"

namespace raytracer_cu {
  class SphereShader : public Shader {
  public:
    // A ray "ray" excites a point "is" in the scene "scene".
    // The function should compute the color of the point by emitting additional
    // rays. To emit the rays, the objects in the scene should be considered
    // (another objects or light sources) so this class dependes on the scene.
    CUDA_HOSTDEV SphereShader(float3 color) : Shader(color){};
    CUDA_HOSTDEV virtual float3 shade(Scene * scene,  Ray &incidentRay,
                             Intersection &intersection)  = 0;
  };

  class GenericSphereShader : public SphereShader {
  public:
    CUDA_HOSTDEV GenericSphereShader(float3 color) : SphereShader(color){};
    CUDA_HOSTDEV float3 shade(Scene * scene,  Ray &incidentRay,
                     Intersection &intersection) ;
  };

  class Sphere : public Object {
  public:
    SphereShader* shader;
    float3 center;
    float r;

    CUDA_HOSTDEV Sphere(float3 center, float r, float3 color)
        : center(center), r(r), Object(color){};
    CUDA_HOSTDEV float3 excite(Scene * scene,  Ray &incidentRay,
                    Intersection &intersection) ;
    CUDA_HOSTDEV void setShader(SphereShader* sphereShader);
    CUDA_HOSTDEV bool intersect( Ray &ray, float3 &outIntersectionPoint, float3 &n,
                  float3 &c);
    CUDA_HOSTDEV void transform( mat3x3 &transformMatrix);
  };
}

#endif
