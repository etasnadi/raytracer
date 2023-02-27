#include <vector>

#include <glm/glm.hpp>

#include "room_scene.h"

#include "triangle.h"
#include "sphere.h"
#include "models.h"

void RoomScene::buildScene(){
    Models models;

    objects = std::vector<std::shared_ptr<Object>>();

    std::vector<std::shared_ptr<Triangle>> box_objects = models.createBox(glm::vec3(0., 0., 0.), 256.);
    int idx = 0;
    for(std::shared_ptr<Triangle> o : box_objects){
        idx += 1;
        auto boxSidesMaterial = std::make_shared<GenericTriangleShader>(o->color);
        if(idx < 5){
            boxSidesMaterial -> setProfile(.9, .1, 0.);
        }
        boxSidesMaterial -> enableShadows = false;
        o->setShader(boxSidesMaterial);
        objects.push_back(o);
    }

    float sqw = 64.f;
    std::vector<std::shared_ptr<Triangle>> transparentSquare = models.square(
        glm::vec3(-sqw, -sqw, 0.),
        glm::vec3(-sqw, sqw, 0.),
        glm::vec3(sqw, sqw, 0.),
        glm::vec3(sqw, -sqw, 0.),
        Colors::green
    );

    auto transparentSquareMaterial = std::make_shared<GenericTriangleShader>(Colors::green);
    transparentSquareMaterial -> setProfile(0., 0., 1.);
    transparentSquareMaterial -> enableShadows = false;
    transparentSquareMaterial -> refractiveIndex = 1.5;
    
    
    transparentSquare[0]->setShader(transparentSquareMaterial);
    transparentSquare[1]->setShader(transparentSquareMaterial);

    objects.push_back(transparentSquare[0]);
    objects.push_back(transparentSquare[1]);

    auto s1 = std::make_shared<Sphere>(glm::vec3(128., 0., 128.), 96., Colors::white);
    auto shinySphereMaterial = std::make_shared<GenericSphereShader>(Colors::green);
    shinySphereMaterial -> setProfile(.05, .95, 0.0);
    s1->setShader(shinySphereMaterial);

    auto s2 = std::make_shared<Sphere>(glm::vec3(-128., 0., 64.), 42., Colors::white);
    auto glassSphereMaterial = std::make_shared<GenericSphereShader>(Colors::green);
    glassSphereMaterial -> setProfile(0., .1, .9);
    glassSphereMaterial -> refractiveIndex = 1.15;
    glassSphereMaterial -> enableShadows = false;
    s2 -> setShader(glassSphereMaterial);

    auto s3 = std::make_shared<Sphere>(glm::vec3(0, 64., 64.), 32., Colors::white);
    auto matteSphereMaterial = std::make_shared<GenericSphereShader>(Colors::green);
    matteSphereMaterial -> setProfile(1., .0, .0);
    matteSphereMaterial -> enableShadows = false;
    s3 -> setShader(matteSphereMaterial);

    auto s4 = std::make_shared<Sphere>(glm::vec3(0, 128., 128.), 32., Colors::white);
    auto slightlyReflectiveSphereMaterial = std::make_shared<GenericSphereShader>(Colors::blue);
    slightlyReflectiveSphereMaterial -> setProfile(.8, .2, .0);
    slightlyReflectiveSphereMaterial -> enableShadows = true;
    s4 -> setShader(slightlyReflectiveSphereMaterial);

    objects.push_back(s1);
    objects.push_back(s2);
    objects.push_back(s3);
    objects.push_back(s4);

    lights = std::vector<std::shared_ptr<Light>>();
    lights.push_back(std::make_shared<Light>(glm::vec3(0, 0, -128.), Colors::white));
}