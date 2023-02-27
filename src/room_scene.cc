#include <iostream>
#include <vector>

#include <glm/glm.hpp>

#include "room_scene.h"

#include "models.h"
#include "sphere.h"
#include "triangle.h"

void RoomScene::buildScene() {
  Models models;

  sceneObjects = std::vector<std::shared_ptr<Object>>();

  // Create the box
  std::vector<glm::vec3> boxSideColors = {
      Colors::green,  Colors::red,     Colors::blue,
      Colors::yellow, Colors::magenta, Colors::cyan,
  };

  std::vector<std::shared_ptr<Triangle>> boxTriangles =
      models.createBox(glm::vec3(0.0f, 0.0f, 0.0f), 256.0f, boxSideColors);
  int idx = 0;

  for (std::shared_ptr<Triangle> boxTriangle : boxTriangles) {
    auto boxSideMaterial =
        std::make_shared<GenericTriangleShader>(boxTriangle->color);
    if (idx <= 8) {
      boxSideMaterial->setProfile(0.9f, 0.1f, 0.0f);
    }
    boxSideMaterial->enableShadows = false;

    if (idx == 4 * 2 ||
        idx == 4 * 2 + 1) { // Floor triangles, apply the floor texture
      if (textures.size() > 0) {
        boxSideMaterial->texture = textures[0];
      }
    }
    boxTriangle->setShader(boxSideMaterial);
    sceneObjects.push_back(boxTriangle);
    idx += 1;
  }

  // Create the square
  float squareSize = 64.f;
  std::vector<std::shared_ptr<Triangle>> transparentSquare =
      models.createSquare(glm::vec3(-squareSize, -squareSize, 0.0f),
                          glm::vec3(-squareSize, squareSize, 0.0f),
                          glm::vec3(squareSize, squareSize, 0.0f),
                          glm::vec3(squareSize, -squareSize, 0.0f),
                          Colors::green);

  auto transparentSquareMaterial =
      std::make_shared<GenericTriangleShader>(Colors::green);
  transparentSquareMaterial->setProfile(0.0f, 0.0f, 1.0f);
  transparentSquareMaterial->enableShadows = false;
  transparentSquareMaterial->refractiveIndex = 1.5f;

  transparentSquare[0]->setShader(transparentSquareMaterial);
  transparentSquare[1]->setShader(transparentSquareMaterial);

  sceneObjects.push_back(transparentSquare[0]);
  sceneObjects.push_back(transparentSquare[1]);

  // Create the spheres
  auto shinySphere = std::make_shared<Sphere>(glm::vec3(128.0f, 0.0f, 128.0f),
                                              96.0f, Colors::white);
  auto shinySphereMaterial =
      std::make_shared<GenericSphereShader>(Colors::green);
  shinySphereMaterial->setProfile(0.05f, 0.95f, 0.0f);
  shinySphere->setShader(shinySphereMaterial);

  auto glassSphere = std::make_shared<Sphere>(glm::vec3(-128.0f, 0.0f, 64.0f),
                                              42.0f, Colors::white);
  auto glassSphereMaterial =
      std::make_shared<GenericSphereShader>(Colors::green);
  glassSphereMaterial->setProfile(0.0f, 0.1f, 0.9f);
  glassSphereMaterial->refractiveIndex = 1.15f;
  glassSphereMaterial->enableShadows = false;
  glassSphere->setShader(glassSphereMaterial);

  auto matteSphere = std::make_shared<Sphere>(glm::vec3(0.0f, 64.0f, 64.0f),
                                              32.0f, Colors::white);
  auto matteSphereMaterial =
      std::make_shared<GenericSphereShader>(Colors::green);
  matteSphereMaterial->setProfile(1.0f, 0.0f, 0.0f);
  matteSphereMaterial->enableShadows = false;
  matteSphere->setShader(matteSphereMaterial);

  auto slightlyShinySphere = std::make_shared<Sphere>(
      glm::vec3(0.0f, 128.0f, 128.0f), 32.0f, Colors::white);
  auto slightlyReflectiveSphereMaterial =
      std::make_shared<GenericSphereShader>(Colors::blue);
  slightlyReflectiveSphereMaterial->setProfile(0.8f, 0.2f, 0.0f);
  slightlyReflectiveSphereMaterial->enableShadows = true;
  slightlyShinySphere->setShader(slightlyReflectiveSphereMaterial);

  sceneObjects.push_back(shinySphere);
  sceneObjects.push_back(glassSphere);
  sceneObjects.push_back(matteSphere);
  sceneObjects.push_back(slightlyShinySphere);

  // Add lights
  lights = std::vector<std::shared_ptr<Light>>();
  lights.push_back(
      std::make_shared<Light>(glm::vec3(0.0f, 0.0f, -128.0f), Colors::white));
}