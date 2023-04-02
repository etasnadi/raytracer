#include "sphere.h"

#include <vector>

#include "raytracer_basics.h"

namespace raytracer_cu {
  CUDA_HOSTDEV bool RayIntersectsSphere( Ray &ray,  Sphere &sph,
                          float3 &intersectionClose,
                          float3 &intersectionFar, float &d1, float &d2) {
    float3 uhat = norm(ray.direction);               // u'
    float nabla_1 = dot(uhat, ray.origin - sph.center); // u'*(o-c)
    float nabla_2a = length(ray.origin - sph.center);   // ||o-c||
    float nabla = (nabla_1 * nabla_1) - (nabla_2a * nabla_2a - (sph.r) * (sph.r));

    if (nabla < 0) {
      return false;
    } else {
      d1 = -nabla_1 - sqrt(nabla); // Closer intersection to the ray origin
      float3 outIntersectionPoint1 = ray.origin + d1 * uhat;

      d2 = -nabla_1 + sqrt(nabla); // Farther intersection to the ray origin
      float3 outIntersectionPoint2 = ray.origin + d2 * uhat;

      intersectionClose = outIntersectionPoint1;
      intersectionFar = outIntersectionPoint2;

      return true;
    }
  }

  // ---------- Sphere definitions ----------

  bool Sphere::intersect( Ray &ray, float3 &outIntersectionPoint,
                        float3 &n, float3 &c) {
    /*
    There are 0, 1 or 2 intersections (|' and |").
    In the case of two intersections, they are ordered according to their position
    on the ray ("pos" and "neg").
    * If both intersections behind the ray origin, then no intersect.
    -------|'-------|"---o--->
    * If the neg is behind the origin, but the pos is not, then the origin is
    inside        -------|'---o---|"------->
    *   the sphere, so one intersection is returned.
    * If both intersections far to the origin, then two intersections are
    returned.         ---o---|'-------|"------->
    */
    float3 intersectNeg;
    float3 intersectPos;

    float3 rayVectorU = norm(ray.direction);

    float d1;
    float d2;
    bool intersection = RayIntersectsSphere(ray, *this, intersectNeg, intersectPos, d1, d2);
    if (intersection) {
      float3 n1 = norm(intersectNeg - center);
      float3 n2 = norm(intersectPos - center);
      // 2) Test self intersection
      if (d1 < 0.0f && d2 < 0.0f) {
        return false;
      }

      if (d1 < 0.0f && d2 > 0.0f) {
        outIntersectionPoint = intersectPos;
      }

      if (d1 > 0.0f && d2 > 0.0f) {
        outIntersectionPoint = intersectNeg;
      }

      n = outIntersectionPoint - center;
      n = div(n, length(n));
      c = color;
      return true;
    }
    return intersection;
  }

  void Sphere::transform( mat3x3 &transformMatrix) {
    center = mm<3>(transformMatrix, center);
  }
 
  float3 Sphere::excite(Scene * scene,
                           Ray &incidentRay,
                          Intersection &intersection)  {
    if (shader) {
      return shader->shade(scene, incidentRay, intersection);
    } else {
      return color;
    }
  }

  void Sphere::setShader(SphereShader* sphereShader) { this->shader = sphereShader; }

  float3 GenericSphereShader::shade(Scene * scene,
                                       Ray &incidentRay,
                                       Intersection &intersection)  {
      
    float3 diffuseComponent = make_float3(0.0f, 0.0f, 0.0f);
    float3 reflectiveComponent = make_float3(0.0f, 0.0f, 0.0f);
    float3 refractiveComponent = make_float3(0.0f, 0.0f, 0.0f);

    Scene * s = scene;

    if (incidentRay.bounces) {
      bool inside = false;
      
      Sphere* obj = (Sphere*) intersection.object;
      if (obj->r > length(incidentRay.origin - obj->center)) {
        inside = true;
      }

      if (reflectedWeight > weightThreshold) {
        if (!inside) {
          computeReflectiveComponent(s, incidentRay, intersection, reflectiveComponent);
        }
      }

      if (refractedWeight > weightThreshold) {
        float localRefractiveIndex = refractiveIndex;

        if (inside) {
          localRefractiveIndex = 1.0f / refractiveIndex;
        }
        computeRefractiveComponent(s, incidentRay, intersection, localRefractiveIndex,
                                  refractiveComponent);
      }
    }
    if (diffuseWeight > weightThreshold) {
      float3 fixedSurfPt = intersection.surfacePoint + .1f * intersection.surfaceNormal;
      diffuseComponent = s->computeDiffuseComponent(
          fixedSurfPt, intersection.surfaceNormal, color, enableShadows);
    }
    return diffuseWeight * diffuseComponent +
          reflectedWeight * reflectiveComponent +
          refractedWeight * refractiveComponent;
  }
}
