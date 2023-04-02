#ifndef MODELS_H
#define MODELS_H

#include <memory>
#include <utility>
#include <vector>

#include "sphere.h"
#include "triangle.h"

namespace raytracer_cu {

class Models {
public:
  CUDA_HOSTDEV static EasyVector<Triangle *> createSquare(float3 topLeft, float3 topRight, float3 bottomRight,
                float3 bottomLeft, const float3 &color);
  CUDA_HOSTDEV static EasyVector<Triangle *> createBox(float3 center, float edgeSize,
                                               EasyVector<float3> &colors);
};

} // namespace raytracer_cu

#endif