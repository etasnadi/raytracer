#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <memory>

#include "cudastuff.h"
#include "math.h"
#include "raytracer_basics.h"
#include "shader.h"

namespace raytracer_cu {
class TriangleShader : public Shader {
public:
  // A ray "ray" excites a point "is" in the scene "scene".
  // The function should compute the color of the point by emitting additional
  // rays. To emit the rays, the objects in the scene should be considered
  // (another objects or light sources) so this class dependes on the scene.
  CUDA_HOSTDEV TriangleShader(float3 color) : Shader(color) {}
  CUDA_HOSTDEV virtual float3 shade(Scene *scene, Ray &incidentRay,
                                    Intersection &intersection, float3 vertex0,
                                    float3 vertex1, float3 vertex2, float2 texCoord0, float2 texCoord1, float2 texCoord2) = 0;
};

class GenericTriangleShader : public TriangleShader {
public:
  ColorBuffer<float3> *texture;
  ColorBuffer<float3> *normals;
  CUDA_HOSTDEV GenericTriangleShader(float3 color) : TriangleShader(color){};
  CUDA_HOSTDEV float3 shade(Scene *scene, Ray &incidentRay,
                            Intersection &intersection, float3 vertex0,
                            float3 vertex1, float3 vertex2, float2 texCoord0,
                            float2 texCoord1, float2 texCoord2);
};

class Triangle : public Object {
public:
  TriangleShader *shader;
  float3 vertex0;
  float3 vertex1;
  float3 vertex2;
  float2 texCoord0;
  float2 texCoord1;
  float2 texCoord2;
  float3 normal_;

  CUDA_HOSTDEV Triangle(float3 vertex0, float3 vertex1, float3 vertex2,
                        float3 color)
      : vertex0(vertex0), vertex1(vertex1), vertex2(vertex2), Object(color) {}
  CUDA_HOSTDEV void setShader(TriangleShader *triangleShader);
  CUDA_HOSTDEV float3 excite(Scene *scene, Ray &incidentRay,
                             Intersection &intersection);
  CUDA_HOSTDEV bool intersect(Ray &incidentRay, float3 &intersectionPoint,
                              float3 &surfaceNormal, float3 &surfaceColor);
  CUDA_HOSTDEV void transform(mat3x3 &transformMatrix);
  CUDA_HOSTDEV float3 normal();
};
} // namespace raytracer_cu

#endif
