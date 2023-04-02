#include "room_scene.h"

#include <iostream>
#include <vector>

#include "basic_types.h"
#include "cudastuff.h"
#include "models.h"
#include "sphere.h"
#include "triangle.h"

namespace raytracer_cu {

CUDA_HOSTDEV void RoomScene::buildScene() {
  sceneObjects = EasyVector<Object *>();

  // Create the box
  EasyVector<float3> boxSideColors;

  boxSideColors.push_back(make_float3(0.0f, 1.0f, 0.0f));
  boxSideColors.push_back(make_float3(1.0f, 0.0f, 0.0f));
  boxSideColors.push_back(make_float3(0.0f, 0.0f, 1.0f));
  boxSideColors.push_back(make_float3(0.0f, 1.0f, 1.0f));
  boxSideColors.push_back(make_float3(1.0f, 0.0f, 1.0f));
  boxSideColors.push_back(make_float3(0.0f, 1.0f, 1.0f));

  EasyVector<Triangle *> boxTriangles =
      Models::createBox(make_float3(0.0f, 0.0f, 0.0f), 256.0f, boxSideColors);

  for (int i = 0; i < boxTriangles.size(); i++) {
    auto boxSideMaterial = new GenericTriangleShader(boxTriangles[i]->color);

    boxSideMaterial->setProfile(0.85f, 0.15f, 0.0f);
    boxTriangles[i]->normal_ = boxTriangles[i]->normal();

    if (i == 4 * 2 || i == 4 * 2 + 1) { // Floor triangles, apply the floor texture
      boxSideMaterial->setProfile(0.8f, 0.2f, 0.0f);
      if (textures.size() > 0) {
        boxSideMaterial->texture = textures[0];
      }
    }

    if (
      i == 1 * 2 || i == 1 * 2 + 1 || 
      i == 0 * 2 || i == 0 * 2 + 1 ||
      i == 2 * 2 || i == 2 * 2 + 1
      ) { // Floor triangles, apply the floor texture
      boxSideMaterial->setProfile(1.0f, 0.0f, 0.0f);
      if (textures.size() > 0) {
        boxSideMaterial->texture = textures[1];
        boxSideMaterial->normals = textures[2];
      }
    }

    if (i == 3 * 2 || i == 3 * 2 + 1) {
      boxSideMaterial->setProfile(0.2f, 0.8f, 0.0f);
    }

    if (i == 5 * 2 || i == 5 * 2 + 1) {
      boxSideMaterial->setProfile(1.0f, 0.0f, 0.0f);
      boxSideMaterial->texture = textures[2];
    }

    boxSideMaterial->enableShadows = false;
    boxTriangles[i]->setShader(boxSideMaterial);
    sceneObjects.push_back(boxTriangles[i]);
  }

  // Create the transparent square
  float squareSize = 64.f;
  EasyVector<Triangle *> transparentSquare =
      Models::createSquare(make_float3(-squareSize, -squareSize, 0.0f),
                    make_float3(-squareSize, squareSize, 0.0f),
                    make_float3(squareSize, squareSize, 0.0f),
                    make_float3(squareSize, -squareSize, 0.0f),
                    make_float3(0.0f, 1.0f, 0.0f));

  auto transparentSquareMaterial =
      new GenericTriangleShader(make_float3(0.0f, 1.0f, 0.0f));
  transparentSquareMaterial->setProfile(0.0f, 0.0f, 1.0f);
  transparentSquareMaterial->enableShadows = false;
  transparentSquareMaterial->refractiveIndex = 1.5f;

  transparentSquare[0]->setShader(transparentSquareMaterial);
  transparentSquare[1]->setShader(transparentSquareMaterial);

  sceneObjects.push_back(transparentSquare[0]);
  sceneObjects.push_back(transparentSquare[1]);

  // Create the spheres
  auto shinySphere = new Sphere(make_float3(128.0f, 0.0f, 128.0f), 96.0f,
                                make_float3(1.0f, 1.0f, 1.0f));
  auto shinySphereMaterial =
      new GenericSphereShader(make_float3(1.0f, 1.0f, 1.0f));
  shinySphereMaterial->setProfile(0.05f, 1.0f, 0.0f);
  shinySphere->setShader(shinySphereMaterial);

  auto glassSphere = new Sphere(make_float3(-128.0f, 0.0f, 64.0f), 42.0f,
                                make_float3(1.0f, 1.0f, 1.0f));
  auto glassSphereMaterial =
      new GenericSphereShader(make_float3(0.0f, 1.0f, 0.0f));
  glassSphereMaterial->setProfile(0.0f, 0.1f, 0.9f);
  glassSphereMaterial->refractiveIndex = 1.15f;
  glassSphereMaterial->enableShadows = false;
  glassSphere->setShader(glassSphereMaterial);

  auto matteSphere = new Sphere(make_float3(0.0f, 64.0f, 64.0f), 32.0f,
                                make_float3(1.0f, 1.0f, 1.0f));
  auto matteSphereMaterial =
      new GenericSphereShader(make_float3(1.0f, 1.0f, 1.0f));
  matteSphereMaterial->setProfile(1.0f, 0.0f, 0.0f);
  matteSphereMaterial->enableShadows = false;
  matteSphere->setShader(matteSphereMaterial);

  auto slightlyShinySphere = new Sphere(make_float3(0.0f, 128.0f, 128.0f),
                                        32.0f, make_float3(1.0f, 1.0f, 1.0f));
  auto slightlyReflectiveSphereMaterial =
      new GenericSphereShader(make_float3(0.0f, 0.0f, 1.0f));
  slightlyReflectiveSphereMaterial->setProfile(0.7f, 0.25f, 0.05f);
  slightlyReflectiveSphereMaterial->enableShadows = true;
  slightlyShinySphere->setShader(slightlyReflectiveSphereMaterial);

  sceneObjects.push_back(shinySphere);
  sceneObjects.push_back(glassSphere);
  sceneObjects.push_back(matteSphere);
  sceneObjects.push_back(slightlyShinySphere);

  // Add lights
  lights.push_back(new Light(make_float3(0.0f, 0.0f, -128.0f),
                             make_float3(1.0f, 1.0f, 1.0f)));
}
} // namespace raytracer_cu
