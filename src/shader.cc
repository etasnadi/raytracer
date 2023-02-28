#include "shader.h"

#include <iostream>
#include <memory>

#include "raytracer_basics.h"

namespace raytracer {

    glm::vec3 computeRafractionDirection(const Ray &incidentRay,
                                        const glm::vec3 &surfaceNormal,
                                        float &kn) {
    glm::vec3 projectedVector =
        incidentRay.direction /
        std::abs(
            glm::dot(incidentRay.direction, surfaceNormal)); // V' in the paper
    float kf =
        1.0f / std::sqrt(square(kn) * square(glm::length(projectedVector)) -
                        square(glm::length(projectedVector + surfaceNormal)));
    return kf * (surfaceNormal + projectedVector) -
            surfaceNormal; // P in the paper
    }

    glm::vec3 computeReflectionDirection(const Ray &incidentRay,
                                        const glm::vec3 &surfaceNormal) {
    glm::vec3 projectedVector =
        incidentRay.direction /
        std::abs(
            glm::dot(incidentRay.direction, surfaceNormal)); // V' in the paper
    return projectedVector + 2.0f * surfaceNormal;           // R in the paper
    }

    bool Shader::computeRefractiveComponent(std::shared_ptr<Scene const> scene,
                                            const Ray &incidentRay,
                                            const Intersection &surfaceIntersection,
                                            float refractiveIndex,
                                            glm::vec3 &refractedColor) const {
    float adjustNormSign = 1.0f;
    if (glm::dot(surfaceIntersection.surfaceNormal, incidentRay.direction) >
        -0.001f) {
        adjustNormSign = -1.0f;
    }

    glm::vec3 adjustedNormal = adjustNormSign * surfaceIntersection.surfaceNormal;
    glm::vec3 refrDir =
        computeRafractionDirection(incidentRay, adjustedNormal, refractiveIndex);
    glm::vec3 fixedSurfPt =
        surfaceIntersection.surfacePoint - bounceSurfDist * adjustedNormal;
    Ray refractionRay(fixedSurfPt, refrDir, incidentRay.bounces - 1);
    glm::vec3 tmpRefractedColor;
    bool result = scene->trace(refractionRay, tmpRefractedColor);

    float debugEps = .0001f;
    if (std::abs(incidentRay.direction.x) < debugEps &&
        std::abs(incidentRay.direction.y) < debugEps) {
    #ifdef DEBUG_STDOUT
        std::cout << "Normal: " << adjustedNormal << "Incoming: " << incidentRay
                << " refracted: " << refractionRay
                << " color: " << tmpRefractedColor << " [" << incidentRay.bounces
                << "]" << std::endl;
    #endif
    }

    refractedColor = tmpRefractedColor;
    return result;
    }

    bool Shader::computeReflectiveComponent(std::shared_ptr<Scene const> scene,
                                            const Ray &incidentRay,
                                            const Intersection &surfaceIntersection,
                                            glm::vec3 &reflectedColor) const {
    glm::vec3 fixedSurfPt = surfaceIntersection.surfacePoint +
                            bounceSurfDist * surfaceIntersection.surfaceNormal;
    glm::vec3 refDir = computeReflectionDirection(
        incidentRay, surfaceIntersection.surfaceNormal);

    Ray reflectionRay(fixedSurfPt, refDir, incidentRay.bounces - 1);
    glm::vec3 reflectedColorTmp;

    bool result = scene->trace(reflectionRay, reflectedColorTmp);
    reflectedColor = reflectedColorTmp;
    return result;
    }
}
