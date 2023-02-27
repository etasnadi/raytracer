#include <iostream>

#include <glm/glm.hpp>

#include "raytracer_basics.h"
#include "cudastuff.h"

CUDA_HOSTDEV bool _closestIntersection(
    const Ray& ray,
    const std::vector<std::shared_ptr<Object>>& objects,
    glm::vec3& is,
    glm::vec3& n,
    glm::vec3& c,
    int& intersectedObjectId){

    float minDist = std::numeric_limits<float>::infinity();
    glm::vec3 minDistIs;
    glm::vec3 minDistN;
    glm::vec3 minDistC;
    int minDistObjectId = -1;

    for(int o_id = 0; o_id < objects.size(); o_id++){
        std::shared_ptr<Object> o = objects[o_id];
        glm::vec3 currIsPoint;
        glm::vec3 currIsN;
        glm::vec3 currIsC;
        if (o->intersect(ray, currIsPoint, currIsN, currIsC)){
            glm::vec3 dd(currIsPoint - ray.origin);
            float currDist = glm::length(dd);
            if (currDist < minDist){
                minDist = currDist;
                minDistIs = currIsPoint;
                minDistC = currIsC;
                minDistN = currIsN;
                minDistObjectId = o_id;
            }
        }
    }
    if(minDistObjectId >= 0){
        is = minDistIs;
        n = minDistN;
        c = minDistC;
        intersectedObjectId = minDistObjectId;
        
        return true;
    }else{
        return false;
    }
}

// ---------- Scene functions ----------

glm::vec3 Scene::computeDiffuseComponent(const glm::vec3& surfPt, const glm::vec3 srufN, const glm::vec3& surfCol, bool shadows){
    glm::vec3 diffuseReflection = glm::vec3(0., 0., 0.);
    for(int l_id = 0; l_id < lights.size(); l_id++){
        std::shared_ptr<Light> l = lights[l_id];
        glm::vec3 surfPtToL = l->lightPosition - surfPt;    
        glm::vec3 surfPtToLU = surfPtToL / glm::length(surfPtToL);
        float angle = glm::dot(surfPtToLU, srufN);

        // Generate shadow ray
        bool occluded = false;
        if (shadows) {
            glm::vec3 occludedObjSurfPt;
            glm::vec3 unused;
            int occludedObjectId;
            Ray shadowRay(surfPt, surfPtToL);
            Intersection closestObject;
            bool occlusion = closestIntersection(shadowRay, closestObject);

            float occludedObjDist = glm::length(closestObject.surfPt - surfPt);

            float lightDist = glm::length(surfPtToL);
            if(occlusion && occludedObjDist < lightDist){
                occluded = true;
            }
        }

        if(angle >= 0. && !occluded){
            diffuseReflection += surfCol*angle;
        }
    }
    return diffuseReflection;
}

CUDA_HOSTDEV bool Scene::closestIntersection(const Ray& ray, Intersection& result){
    glm::vec3 unused;

    int objId;
    bool hit = _closestIntersection(ray, objects, result.surfPt, result.surfNormal, unused, objId);
    if(hit){
        result.obj = objects[objId];
    }
    return hit;
}

CUDA_HOSTDEV  bool Scene::trace(const Ray& ray, glm::vec3& emittedColor){
    Intersection is;
    
    bool hit = closestIntersection(ray, is);
    if(hit){
        emittedColor = is.obj->excite0(shared_from_this(), ray, is);
    }
    return hit;
}

void Scene::transform(glm::mat3x3 trans){
    for(std::shared_ptr<Object> o : objects){
        o->transform(trans);
    }

    for(std::shared_ptr<Light> l : lights){
        l->lightPosition = trans*(l->lightPosition);
    }
}

template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

// ---------- vec3 utility functions ----------

glm::vec3 resize_unit(const glm::vec3& v){
    float l = glm::length(v);
    return glm::vec3(v.x/l, v.y/l, v.z/l);
}

glm::vec3 normalize(const glm::vec3& inp){
    return inp / glm::length(inp);
}

std::ostream& operator<< (std::ostream& os, glm::vec3 o){
    os << "(" << o.x << "," << o.y << "," << o.z << ")";
    return os;
}

// ---------- Ray functions ----------

std::ostream& operator<< (std::ostream& os, Ray r){
    os << "[" << r.origin << " --> " << r.direction << "]";
    return os;
}

// ---------- basic algorithms ----------

float sq(float x){
    return x*x;
}