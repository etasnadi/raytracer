#ifndef RENDERER_H
#define RENDERER_H

#include <memory>

#include "basic_types.h"
#include "raytracer_basics.h"

#include "cuda_runtime.h"
#include "math.h"

namespace raytracer_cu {
typedef Scene* ScenePtr_t;
void checkCudaErr();
void traceCUDA(ColorBuffer<float3> &cb);
CUDA_GLOBAL void initScene(ScenePtr_t* devScenePtr);
CUDA_GLOBAL void traceScene(uint8_t* cDevColorBuffer,
    ScenePtr_t* aScene, 
    float3 viewport_tl,
    float3 viewport_v1,
    float3 viewport_v2,
    float3 eye,
    int2 displaySize);

CUDA_GLOBAL void sceneTransform(mat3x3 transform, ScenePtr_t* aScene);

class Renderer {
private:
  int horizontalDisplacement = 0;
  int verticalDisplacement = 0;
  int horizontalNavigation = 0;
  int verticalNavigation = 0;
  float sensitivity = 0.1f;
  int2 displaySize;
  float3 viewport_tl, viewport_v1, viewport_v2;
  float3 eye;
  CUDA_HOST void modelTransform();
  CUDA_HOST void viewTransform();
  bool first = true;
  EasyVector<int> **devSceneObjects;
  ScenePtr_t *devScenePtr;
  uint8_t *cDevColorBuffer;
  int cColBuffSizeBytes;

public:
  EasyVector<ColorBuffer<float3> *, int> textures;

  CUDA_HOST Renderer(uint32_t screen_width, uint32_t screen_height);
  CUDA_HOST ~Renderer();
  CUDA_HOST void render(uint8_t* frameBuffer);
  CUDA_HOST void addTexture(int texWidth, int texHeight, float3* texData);
  CUDA_HOST void mouseMoveInput(int x, int y);
  CUDA_HOST void mouseWheelInput(int w);
  CUDA_HOST void keyboardArrowsInput(int x, int y);
  CUDA_HOST void buildScene();
};
} // namespace raytracer_cu

#endif