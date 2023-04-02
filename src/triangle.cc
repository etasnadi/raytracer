#include "triangle.h"

#include <memory>

#include "raytracer_basics.h"

namespace raytracer_cu {
/*
Möller–Trumbore algorithm; code is pulled from:
https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
*/
CUDA_HOSTDEV bool RayIntersectsTriangle(Ray &ray, Triangle *inTriangle,
                                        float3 &outIntersectionPoint) {
  const float EPSILON = 0.0000001;

  float3 vertex0 = inTriangle->vertex0;
  float3 vertex1 = inTriangle->vertex1;
  float3 vertex2 = inTriangle->vertex2;
  float3 edge1, edge2, h, s, q;
  float a, f, u, v;
  edge1 = vertex1 - vertex0;
  edge2 = vertex2 - vertex0;
  h = cross(ray.direction, edge2);
  a = dot(edge1, h);
  if (a > -EPSILON && a < EPSILON)
    return false; // This ray is parallel to this triangle.
  f = 1.0 / a;
  s = ray.origin - vertex0;
  float dd = dot(s, h);
  u = f * dd;
  if (u < 0.0 || u > 1.0)
    return false;
  q = cross(s, edge1);
  v = f * dot(ray.direction, q);
  if (v < 0.0 || u + v > 1.0)
    return false;
  // At this stage we can compute t to find out where the intersection point is
  // on the line.
  float dd2 = dot(edge2, q);
  float t = f * dd2;
  if (t > EPSILON) // ray intersection
  {
    outIntersectionPoint = ray.origin + ray.direction * t;
    return true;
  } else {
    // This means that there is a line intersection but not a ray
    // intersection.
    return false;
  }
}

// https://gamedev.stackexchange.com/a/23745
// Compute barycentric coordinates (u, v, w) for
// point p with respect to triangle (a, b, c)
CUDA_HOSTDEV void Barycentric(float3 p, float3 a, float3 b, float3 c, float &u,
                              float &v, float &w) {
  float3 v0 = b - a, v1 = c - a, v2 = p - a;
  float d00 = dot(v0, v0);
  float d01 = dot(v0, v1);
  float d11 = dot(v1, v1);
  float d20 = dot(v2, v0);
  float d21 = dot(v2, v1);
  float denom = d00 * d11 - d01 * d01;
  v = (d11 * d20 - d01 * d21) / denom;
  w = (d00 * d21 - d01 * d20) / denom;
  u = 1.0f - v - w;
}

float3 GenericTriangleShader::shade(Scene *scene, Ray &incidentRay,
                                    Intersection &intersection, float3 vertex0,
                                    float3 vertex1, float3 vertex2, float2 texCoord0, float2 texCoord1, float2 texCoord2) {
  Scene *s = scene;

  float3 diffuseColor = make_float3(0.0f, 0.0f, 0.0f);
  float3 reflectedColor = make_float3(0.0f, 0.0f, 0.0f);
  float3 refractedColor = make_float3(0.0f, 0.0f, 0.0f);

  float weightThreshold = 0.001f;

  float3 fixedSurfacePoint =
      intersection.surfacePoint + 0.1f * intersection.surfaceNormal;
  if (incidentRay.bounces > 0) {
    if (reflectedWeight > weightThreshold) {
      computeReflectiveComponent(s, incidentRay, intersection, reflectedColor);
    }

    if (refractedWeight > weightThreshold) {
      computeRefractiveComponent(s, incidentRay, intersection, refractiveIndex,
                                 refractedColor);
    }
  }

  if (diffuseWeight > weightThreshold) {
    float3 diffuseBaseColor = color;
    if (texture) {

      // Barycentric coefficients
      float vertex0Bar, vertex1Bar, vertex2Bar;
      Barycentric(intersection.surfacePoint, vertex0, vertex1, vertex2,
                  vertex0Bar, vertex1Bar, vertex2Bar);

      // The unnormalized texture sampling coordinate
      float2 sampleCoord = texCoord0 * vertex0Bar + texCoord1 * vertex1Bar +
                           texCoord2 * vertex2Bar;

      // The sampled color (should be bilinear at least)
      float3 textureColor = texture->getPixel(int(sampleCoord.x * texture->w) % texture->w,
                                              int(sampleCoord.y * texture->h) % texture->h);

      diffuseBaseColor = textureColor;
    }

    // Blend the diffuse component.
    diffuseColor = s->computeDiffuseComponent(fixedSurfacePoint,
                                              intersection.surfaceNormal,
                                              diffuseBaseColor, enableShadows);
  }

  return diffuseWeight * diffuseColor + reflectedWeight * reflectedColor +
         refractedWeight * refractedColor;
}

// ---------- Triangle definitions ----------

float3 Triangle::normal() {
  float3 n =
      cross(this->vertex0 - this->vertex2, this->vertex1 - this->vertex0);
  return norm(n);
}

void Triangle::transform(mat3x3 &transformMatrix) {
  vertex0 = mm<3>(transformMatrix, vertex0);
  vertex1 = mm<3>(transformMatrix, vertex1);
  vertex2 = mm<3>(transformMatrix, vertex2);
  normal_ = normal();
}

bool Triangle::intersect(Ray &incidentRay, float3 &intersectionPoint,
                         float3 &surfaceNormal, float3 &surfaceColor) {
  bool is = RayIntersectsTriangle(incidentRay, this, intersectionPoint);
  if (is) {
    surfaceNormal = normal_;
    surfaceColor = color;
  }
  return is;
}

void Triangle::setShader(TriangleShader *triangleShader) {
  this->shader = triangleShader;
}

float3 Triangle::excite(Scene *scene, Ray &incidentRay,
                        Intersection &intersection) {
  if (shader) {
    return shader->shade(scene, incidentRay, intersection, vertex0, vertex1,
                         vertex2, texCoord0, texCoord1, texCoord2);
  } else {
    return color;
  }
}
} // namespace raytracer_cu