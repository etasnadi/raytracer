#include <glm/glm.hpp>

#include "triangle.h"

/*
Möller–Trumbore algorithm; code is pulled from: 
https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
*/
bool RayIntersectsTriangle(const Ray& ray,
                           const Triangle& inTriangle,
                           glm::vec3& outIntersectionPoint)
{
    const float EPSILON = 0.0000001;
    glm::vec3 vertex0 = inTriangle.vertex0;
    glm::vec3 vertex1 = inTriangle.vertex1;  
    glm::vec3 vertex2 = inTriangle.vertex2;
    glm::vec3 edge1, edge2, h, s, q;
    float a,f,u,v;
    edge1 = vertex1 - vertex0;
    edge2 = vertex2 - vertex0;
    h = glm::cross(ray.direction, edge2);
    a = glm::dot(edge1, h);
    if (a > -EPSILON && a < EPSILON)
        return false;    // This ray is parallel to this triangle.
    f = 1.0/a;
    s = ray.origin - vertex0;
    u = f * glm::dot(s, h);
    if (u < 0.0 || u > 1.0)
        return false;
    q = glm::cross(s, edge1);
    v = f * glm::dot(ray.direction, q);
    if (v < 0.0 || u + v > 1.0)
        return false;
    // At this stage we can compute t to find out where the intersection point is on the line.
    float t = f * glm::dot(edge2, q);
    if (t > EPSILON) // ray intersection
    {
        outIntersectionPoint = ray.origin + ray.direction * t;
        return true;
    }
    else // This means that there is a line intersection but not a ray intersection.
        return false;
}

glm::vec3 GenericTriangleShader::shade(std::weak_ptr<Scene> scene, const Ray& incRay, const Intersection& is){
    if(!scene.expired()){
        std::shared_ptr<Scene> s = scene.lock();
        
        glm::vec3 diffuse = Colors::black;
        glm::vec3 reflected = Colors::black;
        glm::vec3 refracted = Colors::black;

        float weightThreshold = 0.001;

        glm::vec3 ss = is.surfPt + .1f * is.surfNormal;
        if (incRay.bounces > 0){
            if (reflectedWeight > weightThreshold){
                computeReflectiveComponent(s, incRay, is, reflected);
            }
            
            if (refractedWeight > weightThreshold){
                computeRefractiveComponent(s, incRay, is, refracted, refractiveIndex);
            }
        }
        // Compute diffuse color
        if (diffuseWeight > weightThreshold){
            diffuse = s -> computeDiffuseComponent(ss, is.surfNormal, color, enableShadows);
        }
        
        //diffuse = _computeDiffuseComponent(s, ss, is.surfNormal, color);
        return diffuseWeight*diffuse + reflectedWeight*reflected + refractedWeight*refracted;
    }
    return color;
}

// ---------- Triangle definitions ----------

glm::vec3 Triangle::normal(){
    glm::vec3 n = glm::cross(this->vertex0-this->vertex2, this->vertex1-this->vertex0);
    return n / glm::length(n);
}

void Triangle::transform(const glm::mat3x3& transformMatrix){
    vertex0 = transformMatrix*vertex0;
    vertex1 = transformMatrix*vertex1;
    vertex2 = transformMatrix*vertex2;
    normal_ = normal();
}

bool Triangle::intersect(const Ray& ray, glm::vec3& outIntersectionPoint, glm::vec3& n, glm::vec3& c){
    bool is = RayIntersectsTriangle(ray, *this, outIntersectionPoint);
    if (is){
        n = normal_;
        c = color;
    }
    return is;
}

void Triangle::setShader(std::shared_ptr<TriangleShader> ts){
    this -> shader = ts;
}

glm::vec3 Triangle::excite0(std::shared_ptr<Scene> scene, const Ray& ray, Intersection& intersection){
    if(shader){
        return shader -> shade(scene, ray, intersection);
    }else{
        return color;
    }
}