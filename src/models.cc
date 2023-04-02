#include "models.h"

#include <memory>
#include <utility>
#include <vector>

#include "basic_types.h"
#include "triangle.h"

namespace raytracer_cu {

EasyVector<Triangle *> Models::createSquare(float3 topLeft, float3 topRight, float3 bottomRight,
              float3 bottomLeft, const float3 &color) {
  Triangle *t1 = new Triangle(topLeft, topRight, bottomLeft, color);
  Triangle *t2 = new Triangle(bottomLeft, topRight, bottomRight, color);

  // Setup texture coordinates
  float pad = 0.1f;
  float minCoord = pad;
  float maxCoord = 1.0f-pad;
  t1 -> texCoord0 = make_float2(minCoord, minCoord);
  t1 -> texCoord1 = make_float2(maxCoord, minCoord);
  t1 -> texCoord2 = make_float2(minCoord, maxCoord);
  
  t2 -> texCoord0 = make_float2(minCoord, maxCoord);
  t2 -> texCoord1 = make_float2(maxCoord, minCoord);
  t2 -> texCoord2 = make_float2(maxCoord, maxCoord);

  EasyVector<Triangle *> triangles;
  triangles.push_back(t1);
  triangles.push_back(t2);

  return triangles;
}

EasyVector<Triangle *> Models::createBox(float3 center, float edgeSize,
                                               EasyVector<float3> &colors) {

  // Top vertices
  float3 topTopLeft = center + make_float3(-edgeSize, -edgeSize, edgeSize);
  float3 topTopRight = center + make_float3(edgeSize, -edgeSize, edgeSize);
  float3 topBottomRight = center + make_float3(edgeSize, -edgeSize, -edgeSize);
  float3 topBottomLeft = center + make_float3(-edgeSize, -edgeSize, -edgeSize);

  // Bottom vertices
  float3 bottomTopLeft = center + make_float3(-edgeSize, edgeSize, edgeSize);
  float3 bottomTopRight = center + make_float3(edgeSize, edgeSize, edgeSize);
  float3 bottomBottomRight =
      center + make_float3(edgeSize, edgeSize, -edgeSize);
  float3 bottomBottomLeft =
      center + make_float3(-edgeSize, edgeSize, -edgeSize);

  // Sides
  auto top = createSquare(topTopLeft, topTopRight, topBottomRight,
                           topBottomLeft, colors[0]); // Green
  auto bottom =
      createSquare(bottomTopLeft, bottomBottomLeft, bottomBottomRight,
                    bottomTopRight, colors[1]); // Red

  auto left = createSquare(topTopLeft, topBottomLeft, bottomBottomLeft,
                            bottomTopLeft, colors[2]); // Blue +
  auto right = createSquare(topTopRight, bottomTopRight, bottomBottomRight,
                             topBottomRight, colors[3]); // Yellow

  auto back = createSquare(topTopLeft, bottomTopLeft, bottomTopRight,
                            topTopRight, colors[4]); // Magenta +
  auto front = createSquare(topBottomLeft, topBottomRight, bottomBottomRight,
                             bottomBottomLeft, colors[5]); // Cyan +

  EasyVector<Triangle *> mesh;

  mesh.push_back(top[0]);
  mesh.push_back(top[1]);
  mesh.push_back(bottom[0]);
  mesh.push_back(bottom[1]);
  mesh.push_back(left[0]);
  mesh.push_back(left[1]);
  mesh.push_back(right[0]);
  mesh.push_back(right[1]);
  mesh.push_back(back[0]);
  mesh.push_back(back[1]);
  mesh.push_back(front[0]);
  mesh.push_back(front[1]);

  return mesh;
}

} // namespace raytracer_cu