#include "triangle.h"

#include <memory>

#include <glm/glm.hpp>

#include "raytracer_basics.h"

namespace raytracer {
  /*
  Möller–Trumbore algorithm; code is pulled from:
  https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
  */
  bool RayIntersectsTriangle(const Ray &ray, const Triangle &inTriangle,
                            glm::vec3 &outIntersectionPoint) {
    const float EPSILON = 0.0000001;
    glm::vec3 vertex0 = inTriangle.vertex0;
    glm::vec3 vertex1 = inTriangle.vertex1;
    glm::vec3 vertex2 = inTriangle.vertex2;
    glm::vec3 edge1, edge2, h, s, q;
    float a, f, u, v;
    edge1 = vertex1 - vertex0;
    edge2 = vertex2 - vertex0;
    h = glm::cross(ray.direction, edge2);
    a = glm::dot(edge1, h);
    if (a > -EPSILON && a < EPSILON)
      return false; // This ray is parallel to this triangle.
    f = 1.0 / a;
    s = ray.origin - vertex0;
    u = f * glm::dot(s, h);
    if (u < 0.0 || u > 1.0)
      return false;
    q = glm::cross(s, edge1);
    v = f * glm::dot(ray.direction, q);
    if (v < 0.0 || u + v > 1.0)
      return false;
    // At this stage we can compute t to find out where the intersection point is
    // on the line.
    float t = f * glm::dot(edge2, q);
    if (t > EPSILON) // ray intersection
    {
      outIntersectionPoint = ray.origin + ray.direction * t;
      return true;
    } else // This means that there is a line intersection but not a ray
          // intersection.
      return false;
  }

  // https://gamedev.stackexchange.com/a/23745
  // Compute barycentric coordinates (u, v, w) for
  // point p with respect to triangle (a, b, c)
  void Barycentric(const glm::vec3 p, const glm::vec3 a, const glm::vec3 b,
                  const glm::vec3 c, float &u, float &v, float &w) {
    glm::vec3 v0 = b - a, v1 = c - a, v2 = p - a;
    float d00 = glm::dot(v0, v0);
    float d01 = glm::dot(v0, v1);
    float d11 = glm::dot(v1, v1);
    float d20 = glm::dot(v2, v0);
    float d21 = glm::dot(v2, v1);
    float denom = d00 * d11 - d01 * d01;
    v = (d11 * d20 - d01 * d21) / denom;
    w = (d00 * d21 - d01 * d20) / denom;
    u = 1.0f - v - w;
  }

  glm::vec3 GenericTriangleShader::shade(std::weak_ptr<Scene const> scene,
                                        const Ray &incidentRay,
                                        const Intersection &intersection,
                                        glm::vec3 vertex0, glm::vec3 vertex1,
                                        glm::vec3 vertex2) const {
    if (!scene.expired()) {
      std::shared_ptr<Scene const> s = scene.lock();

      glm::vec3 diffuseColor = Colors::black;
      glm::vec3 reflectedColor = Colors::black;
      glm::vec3 refractedColor = Colors::black;

      float weightThreshold = 0.001f;

      glm::vec3 fixedSurfacePoint =
          intersection.surfacePoint + 0.1f * intersection.surfaceNormal;
      if (incidentRay.bounces > 0) {
        if (reflectedWeight > weightThreshold) {
          computeReflectiveComponent(s, incidentRay, intersection,
                                    reflectedColor);
        }

        if (refractedWeight > weightThreshold) {
          computeRefractiveComponent(s, incidentRay, intersection,
                                    refractiveIndex, refractedColor);
        }
      }

      if (diffuseWeight > weightThreshold) {
        glm::vec3 diffuseBaseColor = color;
        if (texture) {
          // Triangle vertices mapped to the texture
          glm::vec2 anchor0(0.1f, 0.1f);
          glm::vec2 anchor1(0.5f, 0.9f);
          glm::vec2 anchor2(0.9f, 0.1f);

          // Barycentric coefficients
          float vertex0Bar, vertex1Bar, vertex2Bar;
          Barycentric(intersection.surfacePoint, vertex0, vertex1, vertex2,
                      vertex0Bar, vertex1Bar, vertex2Bar);

          // The unnormalized texture sampling coordinate
          glm::vec2 sampleCoord =
              anchor0 * vertex0Bar + anchor1 * vertex1Bar + anchor2 * vertex2Bar;

          // The sampled color (should be bilinear at least)
          glm::vec3 textureColor =
              texture->getPixel(int(sampleCoord.x * texture->w),
                                int(sampleCoord.y * texture->h));

          textureColor /= 255.0f;
          diffuseBaseColor = textureColor;
        }

        // Blend the diffuse component.
        diffuseColor = s->computeDiffuseComponent(
            fixedSurfacePoint, intersection.surfaceNormal, diffuseBaseColor,
            enableShadows);
      }

      return diffuseWeight * diffuseColor + reflectedWeight * reflectedColor +
            refractedWeight * refractedColor;
    }
    return color;
  }

  // ---------- Triangle definitions ----------

  glm::vec3 Triangle::normal() {
    glm::vec3 n =
        glm::cross(this->vertex0 - this->vertex2, this->vertex1 - this->vertex0);
    return n / glm::length(n);
  }

  void Triangle::transform(const glm::mat3x3 &transformMatrix) {
    vertex0 = transformMatrix * vertex0;
    vertex1 = transformMatrix * vertex1;
    vertex2 = transformMatrix * vertex2;
    normal_ = normal();
  }

  bool Triangle::intersect(const Ray &incidentRay, glm::vec3 &intersectionPoint,
                          glm::vec3 &surfaceNormal, glm::vec3 &surfaceColor) {
    bool is = RayIntersectsTriangle(incidentRay, *this, intersectionPoint);
    if (is) {
      surfaceNormal = normal_;
      surfaceColor = color;
    }
    return is;
  }

  void Triangle::setShader(std::shared_ptr<TriangleShader> triangleShader) {
    this->shader = triangleShader;
  }

  glm::vec3 Triangle::excite(std::shared_ptr<Scene const> scene,
                            const Ray &incidentRay,
                            Intersection &intersection) const {
    if (shader) {
      return shader->shade(scene, incidentRay, intersection, vertex0, vertex1,
                          vertex2);
    } else {
      return color;
    }
  }
}
