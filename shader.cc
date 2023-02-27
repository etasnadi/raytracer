#include <iostream>
#include <memory>

#include "raytracer_basics.h"
#include "shader.h"

glm::vec3 rafractionDir(const Ray& incRay, const glm::vec3& normal, float& kn){
    glm::vec3 proj = incRay.direction/std::abs(glm::dot(incRay.direction, normal)); // V' in the paper
    float kf = 1.f / std::sqrt(
        sq(kn) * sq(glm::length(proj)) - 
        sq(glm::length(proj + normal))
    );
    return kf * (normal + proj) - normal;                 // P in the paper
}

glm::vec3 reflectionDir(const Ray& incRay, const glm::vec3& surfNorm){   
    glm::vec3 proj = incRay.direction/std::abs(glm::dot(incRay.direction, surfNorm));  // V' in the paper
    return proj + 2.f*surfNorm;                                            // R in the paper
}

bool Shader::computeRefractiveComponent(std::shared_ptr<Scene> scene, const Ray& incRay, const Intersection& is, glm::vec3& refracted, float refractiveIndex){
    float adjustNormSign = 1.;
    if (glm::dot(is.surfNormal, incRay.direction) > -0.001){
        adjustNormSign = -1.;
    }

    glm::vec3 adjustedNormal = adjustNormSign*is.surfNormal;
    glm::vec3 refrDir = rafractionDir(incRay, adjustedNormal, refractiveIndex);
    glm::vec3 fixedSurfPt = is.surfPt - bounceSurfDist * adjustedNormal;
    Ray refractionRay(fixedSurfPt, refrDir, incRay.bounces - 1);
    glm::vec3 ref;
    bool result = scene->trace(refractionRay, ref);
    // debug
    float eps = .0001;
    if (std::abs(incRay.direction.x) < eps && std::abs(incRay.direction.y) < eps){
        std::cout << "Normal: " << adjustedNormal << "Incoming: " << incRay << " refracted: " << refractionRay <<  " color: " << ref << " [" << incRay.bounces << "]" << std::endl;
    }
    
    refracted = ref;
    return result;
}

bool Shader::computeReflectiveComponent(std::shared_ptr<Scene> scene, const Ray& incRay, const Intersection& is, glm::vec3& reflected){
    glm::vec3 fixedSurfPt = is.surfPt + bounceSurfDist * is.surfNormal;
    glm::vec3 refDir = reflectionDir(incRay, is.surfNormal);

    Ray reflectionRay(fixedSurfPt, refDir, incRay.bounces - 1);
    glm::vec3 ref;

    bool result = scene->trace(reflectionRay, ref);
    reflected = ref;
    return result;
}