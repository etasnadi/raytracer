#include "renderer.h"

#include <cmath>
#include <cstdint>
#include <iostream>
#include <limits>
#include <memory>
#include <sstream>
#include <vector>

#include "cuda_runtime.h"

#include "basic_types.h"
#include "cudastuff.h"
#include "math.h"
#include "raytracer_basics.h"
#include "room_scene.h"

namespace raytracer_cu {

CUDA_HOSTDEV int idx(int x, int y, int h, int w) {
  return y * w + x;
}

CUDA_GLOBAL void initScene(ScenePtr_t* devScenePtr) {
  int x = threadIdx.x + blockIdx.x * blockDim.x;
  int y = threadIdx.y + blockIdx.y * blockDim.y;
  
  if(x == 0 && y == 0){
    Scene *scene = new RoomScene();
    devScenePtr[0] = scene;
  }
}

CUDA_GLOBAL void _buildScene(ScenePtr_t* devScenePtr) {
  int x = threadIdx.x + blockIdx.x * blockDim.x;
  int y = threadIdx.y + blockIdx.y * blockDim.y;
  
  if(x == 0 && y == 0){
    ScenePtr_t scene = devScenePtr[0];
    scene -> buildScene();
  }
}

CUDA_GLOBAL void sceneTransform(mat3x3 transform, ScenePtr_t* aScene){
  int x = threadIdx.x + blockIdx.x * blockDim.x;
  int y = threadIdx.y + blockIdx.y * blockDim.y;
  
  if(x == 0 && y == 0){
    Scene* scene = aScene[0];
    scene -> transform(transform);
  }
}

CUDA_GLOBAL void _addTexture(ScenePtr_t* devScenePtr, int texWidth, int texHeight, float3* texData){
  int x = threadIdx.x + blockIdx.x * blockDim.x;
  int y = threadIdx.y + blockIdx.y * blockDim.y;
  
  ColorBuffer<float3>* texture = new ColorBuffer<float3>(texWidth, texHeight);
  texture -> c = texData;

  if(x == 0 && y == 0){  
    Scene* scene = devScenePtr[0];
    scene -> textures.push_back(texture);
  }
}

CUDA_GLOBAL void traceScene(uint8_t* cDevColorBuffer,
    ScenePtr_t* aScene, 
    float3 viewport_tl,
    float3 viewport_v1,
    float3 viewport_v2,
    float3 eye,
    int2 displaySize) {
  int x = threadIdx.x + blockIdx.x * blockDim.x;
  int y = threadIdx.y + blockIdx.y * blockDim.y;

  if(x > 0 && x < displaySize.x && y > 0 &&  y < displaySize.y){
    Scene* scene = aScene[0];
    
    float3 screen = viewport_tl +
                        (float(x) / displaySize.x) * viewport_v1 +
                        (float(y) / displaySize.y) * viewport_v2;

    float3 resultCol = make_float3(0.0f, 0.0f, 0.0f);
    
    int maxBounces = 3;
    Ray eyeRay(screen, screen-eye, maxBounces);
    int linIdx = idx(x, y, displaySize.x, displaySize.y);
    
    if (scene->trace(eyeRay, resultCol)) {
      cDevColorBuffer[linIdx*4+1] = uint8_t(__float2int_rn (resultCol.z*255));
      cDevColorBuffer[linIdx*4+2] = uint8_t(__float2int_rn (resultCol.y*255));
      cDevColorBuffer[linIdx*4+3] = uint8_t(__float2int_rn (resultCol.x*255));
    } else {
      cDevColorBuffer[linIdx*4+1] = 0;
      cDevColorBuffer[linIdx*4+2] = 0;
      cDevColorBuffer[linIdx*4+3] = 0;
    }
    cDevColorBuffer[linIdx*4+0] = 255;
  }
}

void checkCudaErr() {
  cudaDeviceSynchronize();
  cudaError_t err;
  err = cudaGetLastError();
  if (err != cudaSuccess) {
    std::cout << cudaGetErrorString(err) << std::endl;
  }
}

void Renderer::addTexture(int texWidth, int texHeight, float3* texData){
  
  float3* dTexData;
  int nTexBytes = texWidth*texHeight*sizeof(float3);
  cudaMalloc((void**)&dTexData, nTexBytes);
  cudaMemcpy(dTexData, texData, nTexBytes, cudaMemcpyHostToDevice);
  _addTexture<<<1, 1>>>(devScenePtr, texWidth, texHeight, dTexData);
}

CUDA_HOSTDEV mat3x3 getRotationMatrixX(float rotRad) {
  // Rotates around the X axis
  
  mat3x3 rot;
  rot.data[0][0] = 1.;
  rot.data[1][1] = cos(rotRad);
  rot.data[1][2] = -sin(rotRad);
  rot.data[2][1] = sin(rotRad);
  rot.data[2][2] = cos(rotRad);
  return rot;
}

CUDA_HOSTDEV mat3x3 getRotationMatrixY(float rotRad) {
  // Rotates around the Y axis

  mat3x3 rot;
  rot.data[1][1] = 1.;
  rot.data[0][0] = cos(rotRad);
  rot.data[2][0] = -sin(rotRad);
  rot.data[0][2] = sin(rotRad);
  rot.data[2][2] = cos(rotRad);
  return rot;
}

CUDA_HOSTDEV mat3x3 getRotationMatrixZ(float rotRad) {
  // Rotates around the z axis
  
  mat3x3 rot;
  rot.data[2][2] = 1.;
  rot.data[0][0] = cos(rotRad);
  rot.data[1][0] = -sin(rotRad);
  rot.data[0][1] = sin(rotRad);
  rot.data[1][1] = cos(rotRad);
  return rot;
}

void Renderer::mouseMoveInput(int x, int y){
  horizontalDisplacement = x;
  verticalDisplacement = y;
}

void Renderer::mouseWheelInput(int w){
}

void Renderer::keyboardArrowsInput(int x, int y){
  horizontalNavigation = x;
  verticalNavigation = y;
}

void Renderer::buildScene(){
  _buildScene<<<1, 1>>>(devScenePtr);
}

std::tuple<float3, float3, float3> getViewport(int2 viewport_size,
                                               float viewport_z) {
  // The viewport is an (assumed) square surface that is on the xy and the
  // center is at the origin.
  float3 vp = make_float3(viewport_size.x, viewport_size.y, viewport_z);

  // Define the top-left, top-right, bottom-lroteft points
  float3 viewport_tl = make_float3(-(vp.x / 2.0f), -(vp.y / 2.0f), vp.z);
  float3 viewport_tr = make_float3(vp.x / 2.0f, -(vp.y / 2.0f), vp.z);
  float3 viewport_bl = make_float3(-(vp.x / 2.0f), vp.y / 2.0f, vp.z);

  // Compute the directions
  float3 viewport_v1 = (viewport_tr - viewport_tl);
  float3 viewport_v2 = (viewport_bl - viewport_tl);

  return {viewport_tl, viewport_v1, viewport_v2};
}

Renderer::Renderer(uint32_t screen_width, uint32_t screen_height)
    : textures(EasyVector<ColorBuffer<float3> *, int>(12)) {

  displaySize = make_int2(screen_width, screen_height);

  textures = EasyVector<ColorBuffer<float3> *, int>(12);

  // Eye position
  float eye_z = -200.0f;
  eye = make_float3(0.0f, 0.0f, eye_z);

  // Config viewport
  auto viewport = getViewport(make_int2(256.0f, 256.0f), eye_z + 100.0f);
  std::tie(viewport_tl, viewport_v1, viewport_v2) = viewport;

  // Initialize CUDA stack
  size_t s;
  cudaDeviceSetLimit(cudaLimitStackSize, 1024*30);
  checkCudaErr();

  cudaDeviceGetLimit(&s, cudaLimitStackSize);
  checkCudaErr();
  std::cout << "Stack size: " << s << std::endl;

  // Initialize the color buffer
  cColBuffSizeBytes = displaySize.x * displaySize.y * 4;
  cudaMalloc((void **)&cDevColorBuffer, cColBuffSizeBytes);
  checkCudaErr();

  // Initialize the scene
  cudaMalloc((ScenePtr_t **)&devScenePtr, sizeof(ScenePtr_t));
  initScene<<<1, 1>>>(devScenePtr);
  checkCudaErr();
}

void Renderer::modelTransform() {
  mat3x3 rot1 = getRotationMatrixX(0.);
  mat3x3 rot2 = getRotationMatrixY(0.);
  mat3x3 transform = mm(rot2, rot1);
  sceneTransform<<<1, 1>>>(transform, devScenePtr);
}

void Renderer::viewTransform(){
  // Transform camera and eye

  sensitivity = 500.0f;

  mat3x3 rot1 = getRotationMatrixY(horizontalDisplacement/sensitivity);
  mat3x3 rot2 = getRotationMatrixX(-verticalDisplacement/sensitivity);
  mat3x3 transform = mm(rot2, rot1);

  viewport_tl = mm<3>(transform, viewport_tl);
  viewport_v1 = mm<3>(transform, viewport_v1);
  viewport_v2 = mm<3>(transform, viewport_v2);
  eye = mm<3>(transform, eye);

  horizontalDisplacement = 0;
  verticalDisplacement = 0;
}

void Renderer::render(uint8_t* frameBuffer) {
  modelTransform();
  viewTransform();

  // 32x32 blocks, 16x16 threads per block
  dim3 threadsPerBlock(16, 16);
  dim3 numBlocks(displaySize.x / threadsPerBlock.x,
                 displaySize.y / threadsPerBlock.y);
  
  cudaEvent_t start, stop;
  cudaEventCreate(&start);
  cudaEventCreate(&stop);
  cudaEventRecord(start);

  traceScene<<<numBlocks, threadsPerBlock>>>(cDevColorBuffer, devScenePtr,
                                         viewport_tl, viewport_v1, viewport_v2,
                                         eye, displaySize);
  
  cudaEventRecord(stop);
  cudaEventSynchronize(stop);
  float milliseconds = 0;
  cudaEventElapsedTime(&milliseconds, start, stop);
  checkCudaErr();

  std::cout << "Trace kernel execution time: " << milliseconds << " ms." << std::endl;

  cudaMemcpy(frameBuffer, cDevColorBuffer,
             cColBuffSizeBytes, cudaMemcpyDeviceToHost);

}

Renderer::~Renderer(){
  cudaFree(cDevColorBuffer);
}

} // namespace raytracer_cu
