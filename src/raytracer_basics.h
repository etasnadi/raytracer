#ifndef RAYTRACER_BASICS_H
#define RAYTRACER_BASICS_H

#include <memory>
#include <vector>

#include "basic_types.h"
#include "cudastuff.h"

#include "math.h"
#include "cuda_runtime.h"

#define COLOR_IDX_RED 0
#define COLOR_IDX_GREEN 1
#define COLOR_IDX_BLUE 2
#define COLOR_IDX_YELLOW 3
#define COLOR_IDX_MAGENTA 4
#define COLOR_IDX_CYAN 5
#define COLOR_IDX_BLACK 6
#define COLOR_IDX_GRAY 7
#define COLOR_IDX_WHITE 8

namespace raytracer_cu {

namespace colors {
#ifdef __CUDACC__
extern CUDA_DEVICE_CONSTANT float3 *deviceColorTable;
#else
extern float3 *red;
#endif
} // namespace colors

#ifdef __CUDA_ARCH__

class Colors {
public:
  static float3 red;
  static float3 green;
  static float3 blue;
  static float3 yellow;
  static float3 magenta;
  static float3 cyan;
  static float3 black;
  static float3 gray;
  static float3 white;
};

#endif

class Object;

typedef struct {
  float3 surfacePoint;
  float3 surfaceNormal;
  Object *object;
} Intersection;

class Ray {
public:
  uint32_t bounces;
  float3 origin;
  float3 direction;
  CUDA_HOSTDEV Ray(){};
  CUDA_HOSTDEV Ray(float3 origin, float3 direction, uint32_t bounces = 1)
      : origin(origin), direction(direction), bounces(bounces) {}
};

CUDA_HOSTDEV bool _closestIntersection(Ray &ray, EasyVector<Object *> &objects,
                                       float3 &is, float3 &n,
                                       float3 &c, int &intersectedObjectId);

std::ostream &operator<<(std::ostream &os, Ray r);
std::ostream &operator<<(std::ostream &os, float3 o);
CUDA_HOSTDEV float3 normalize(float3 &inp);
template <typename T> int sgn(T val);

class Light {
public:
  float3 lightPosition;
  float3 lightColor;
  CUDA_HOSTDEV Light(float3 lightPosition, float3 lightColor)
      : lightPosition(lightPosition), lightColor(lightColor) {}
};

class Scene {
public:
  EasyVector<Object *> sceneObjects;
  EasyVector<Light *> lights;
  EasyVector<ColorBuffer<float3> *> textures;
  int nTextures;

  CUDA_HOSTDEV void transform(mat3x3 trans);
  CUDA_HOSTDEV bool closestIntersection(Ray &ray, Intersection &result, bool shadowRay=false);
  CUDA_HOSTDEV bool trace(Ray &ray, float3 &result_color);
  CUDA_HOSTDEV float3 computeDiffuseComponent(float3 &surfPt,
                                                 float3 &srufN,
                                                 float3 &surfCol,
                                                 bool shadows);
  CUDA_HOSTDEV virtual void buildScene() = 0;
};

class Object {
public:
  float3 color;

  CUDA_HOSTDEV Object() {}

  CUDA_HOSTDEV Object(float3 color) : color(color) {}
  CUDA_HOSTDEV virtual bool intersect(Ray &ray, float3 &outIntersectionPoint,
                                      float3 &outNormal,
                                      float3 &outColor) = 0;
  CUDA_HOSTDEV virtual void transform(mat3x3 &transformMatrix) = 0;
  CUDA_HOSTDEV virtual float3 excite(Scene *scene, Ray &incidentRay,
                                        Intersection &intersection) = 0;
};
} // namespace raytracer_cu

#endif
