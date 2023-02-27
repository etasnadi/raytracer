#include <vector>

#include <glm/glm.hpp>

#include "raytracer_basics.h"
#include "sphere.h"

bool RayIntersectsSphere(const Ray& ray, 
                         const Sphere& sph, 
                         glm::vec3& o_outIntersectionClose,
                         glm::vec3& o_outIntersectionFar,
                         float& d1,
                         float& d2){
    glm::vec3 uhat = resize_unit(ray.direction);                                        // u'
    float nabla_1 = glm::dot(uhat, ray.origin - sph.center);                        // u'*(o-c)
    float nabla_2a = glm::length(ray.origin - sph.center);                          // ||o-c||
    float nabla = (nabla_1*nabla_1) - (nabla_2a*nabla_2a - (sph.r) * (sph.r));
    
    if (nabla < 0){
        return false;
    }else{
        d1 = -nabla_1 - std::sqrt(nabla); // Closer intersection to the ray origin
        glm::vec3 outIntersectionPoint1 = ray.origin + d1*uhat;
        
        d2 = -nabla_1 + std::sqrt(nabla); // Farther intersection to the ray origin
        glm::vec3 outIntersectionPoint2 = ray.origin + d2*uhat;

        o_outIntersectionClose = outIntersectionPoint1;
        o_outIntersectionFar = outIntersectionPoint2;

        return true;
    }
}

// ---------- Sphere definitions ----------

bool Sphere::intersect(const Ray& ray, glm::vec3& outIntersectionPoint, glm::vec3& n, glm::vec3& c){
    /*
    There are 0, 1 or 2 intersections (|' and |").
    In the case of two intersections, they are ordered according to their position on the ray ("pos" and "neg").
    * If both intersections behind the ray origin, then no intersect.                       -------|'-------|"---o--->
    * If the neg is behind the origin, but the pos is not, then the origin is inside        -------|'---o---|"------->
    *   the sphere, so one intersection is returned.
    * If both intersections far to the origin, then two intersections are returned.         ---o---|'-------|"------->
    */
    glm::vec3 intersectNeg;
    glm::vec3 intersectPos;

    glm::vec3 rayVectorU = normalize(ray.direction);

    float d1;
    float d2;
    bool is = RayIntersectsSphere(ray, *this, intersectNeg, intersectPos, d1, d2);
    if(is){
        glm::vec3 n1 = normalize(intersectNeg - center);
        glm::vec3 n2 = normalize(intersectPos - center);
        // 2) Test self intersection
        if(d1 < 0 && d2 < 0){
            return false;
        }

        if(d1 < 0 && d2 > 0){
            outIntersectionPoint = intersectPos;
        }

        if(d1 > 0 && d2 > 0){
            outIntersectionPoint = intersectNeg;
        }

        n = outIntersectionPoint - center;
        n = n / glm::length(n);
        c = color;
        return true;
    }
    return is;
}

void Sphere::transform(const glm::mat3x3& transformMatrix){
    center = transformMatrix*center;
}

glm::vec3 Sphere::excite0(std::shared_ptr<Scene> scene, const Ray& ray, Intersection& intersection){
    if(shader){
        return shader -> shade(scene, ray, intersection);
    }else{
        return color;
    }
}

void Sphere::setShader(std::shared_ptr<SphereShader> ss){
    this -> shader = ss;
}

glm::vec3 GenericSphereShader::shade(std::weak_ptr<Scene> scene, const Ray& incRay, const Intersection& is){
    if(!scene.expired()){
        glm::vec3 diffuseComponent = Colors::black;
        glm::vec3 reflectiveComponent = Colors::black;
        glm::vec3 refractiveComponent = Colors::black;
        
        std::shared_ptr<Scene> s = scene.lock();

        if (incRay.bounces){
            bool inside = false;
            std::shared_ptr<Sphere> obj = std::dynamic_pointer_cast<Sphere>(is.obj);
            if(obj->r > glm::length(incRay.origin - obj->center)){
                inside = true;
            }

            if (reflectedWeight > weightThreshold){
                if(!inside){
                    computeReflectiveComponent(s, incRay, is, reflectiveComponent);
                }
            }

            if (refractedWeight > weightThreshold){
                float localRefractiveIndex = refractiveIndex;
                
                if(inside){
                    localRefractiveIndex = 1./refractiveIndex;
                }
                computeRefractiveComponent(s, incRay, is, refractiveComponent, localRefractiveIndex);
            }
        }
        if (diffuseWeight > weightThreshold){
            glm::vec3 fixedSurfPt = is.surfPt + .1f * is.surfNormal;
            diffuseComponent = s -> computeDiffuseComponent(fixedSurfPt, is.surfNormal, color, enableShadows);
        }
        return diffuseWeight*diffuseComponent + reflectedWeight*reflectiveComponent + refractedWeight*refractiveComponent;
    }
    return color;
}
