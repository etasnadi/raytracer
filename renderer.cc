#include <cmath>
#include <iostream>
#include <limits>
#include <memory>
#include <sstream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include "renderer.h"
#include "room_scene.h"

glm::vec3 Colors::red;
glm::vec3 Colors::green;
glm::vec3 Colors::blue;
glm::vec3 Colors::yellow;
glm::vec3 Colors::magenta;
glm::vec3 Colors::cyan;
glm::vec3 Colors::black;
glm::vec3 Colors::gray;
glm::vec3 Colors::white;

template <class T>
ColorBuffer<T>::ColorBuffer(int a_w, int a_h) : w(a_w), h(a_h) {
  c = std::vector<T>(w * h);
}

template <class T> int ColorBuffer<T>::idx(int x, int y) { return y * w + x; }

template <class T> void ColorBuffer<T>::setPixel(int x, int y, T intensity) {
  maxIntensity.x = std::max(maxIntensity.x, intensity.x);
  maxIntensity.y = std::max(maxIntensity.y, intensity.y);
  maxIntensity.z = std::max(maxIntensity.z, intensity.z);

  minIntensity.x = std::min(minIntensity.x, intensity.x);
  minIntensity.y = std::min(minIntensity.y, intensity.y);
  minIntensity.z = std::min(minIntensity.z, intensity.z);

  c[idx(x, y)] = intensity;
}

template <class T> T ColorBuffer<T>::getPixel(int x, int y, bool normalize) {
  glm::vec3 color = c[idx(x, y)];
  if (normalize) {
    return (color - minIntensity) / (color + maxIntensity);
  } else {
    return color;
  }
}

glm::mat3x3 getRotationMatrixX(float rotRad) {
  glm::mat3x3 rot(0.);
  rot[0][0] = 1.;
  rot[1][1] = std::cos(rotRad);
  rot[1][2] = -std::sin(rotRad);
  rot[2][1] = std::sin(rotRad);
  rot[2][2] = std::cos(rotRad);
  return rot;
}

glm::mat3x3 getRotationMatrixY(float rotRad) {
  glm::mat3x3 rot(0.);
  rot[1][1] = 1.;
  rot[0][0] = std::cos(rotRad);
  rot[2][0] = -std::sin(rotRad);
  rot[0][2] = std::sin(rotRad);
  rot[2][2] = std::cos(rotRad);
  return rot;
}

void Renderer::rotateLeft() { ry = 0.5f * sensitivity; }

void Renderer::rotateRight() { ry = -0.5f * sensitivity; }

void Renderer::rotateUp() { rx = 0.5f * sensitivity; }

void Renderer::rotateDown() { rx = -0.5f * sensitivity; }

std::tuple<glm::vec3, glm::vec3, glm::vec3> getViewport(glm::vec2 viewport_size,
                                                        float viewport_z) {
  // The viewport is an (assumed) square surface that is on the xy and the
  // center is at the origin.
  glm::vec3 vp(viewport_size.x, viewport_size.y, viewport_z);

  // Define the top-left, top-right, bottom-lroteft points
  glm::vec3 viewport_tl(-(vp.x / 2.0f), -(vp.y / 2.0f), vp.z);
  glm::vec3 viewport_tr(vp.x / 2.0f, -(vp.y / 2.0f), vp.z);
  glm::vec3 viewport_bl(-(vp.x / 2.0f), vp.y / 2.0f, vp.z);

  // Compute the directions
  glm::vec3 viewport_v1 = (viewport_tr - viewport_tl);
  glm::vec3 viewport_v2 = (viewport_bl - viewport_tl);

  return {viewport_tl, viewport_v1, viewport_v2};
}

template class ColorBuffer<glm::vec3>;

Renderer::Renderer(uint32_t screen_width, uint32_t screen_height) {
  Colors::red = glm::vec3(1.0f, 0.0f, 0.0f);
  Colors::green = glm::vec3(0.0f, 1.0f, 0.0f);
  Colors::blue = glm::vec3(0.0f, 0.0f, 1.0f);
  Colors::yellow = glm::vec3(1.0f, 1.0f, 0.0f);
  Colors::magenta = glm::vec3(1.0f, 0.0f, 1.0f);
  Colors::cyan = glm::vec3(0.0f, 1.0f, 1.0f);
  Colors::black = glm::vec3(0.0f, 0.0f, 0.0f);
  Colors::gray = glm::vec3(0.5f, 0.5f, 0.5f);
  Colors::white = glm::vec3(1.0f, 1.0f, 1.0f);

  displaySize = glm::ivec2(screen_width, screen_height);

  // Eye position
  float eye_z = -200.0f;
  eye = glm::vec3(0.0f, 0.0f, eye_z);

  // Config viewport
  auto viewport = getViewport(glm::vec2(256.0f, 256.0f), eye_z + 100.0f);
  std::tie(viewport_tl, viewport_v1, viewport_v2) = viewport;

  scene = std::make_shared<RoomScene>();
  scene->buildScene();
}

void Renderer::applyInputTransforms() {
  glm::mat3x3 rot1 = getRotationMatrixX(rx);
  glm::mat3x3 rot2 = getRotationMatrixY(ry);
  glm::mat3x3 transform = rot2 * rot1;
  scene->transform(transform);
  rx = 0.0f;
  ry = 0.0f;
}

ColorBuffer<glm::vec3> Renderer::render() {
  applyInputTransforms();

  ColorBuffer<glm::vec3> colBuff(displaySize.x, displaySize.y);

#pragma omp parallel
  {
#pragma omp for
    for (int x = 0; x < displaySize.x; x++) {
      for (int y = 0; y < displaySize.y; y++) {
        glm::vec3 screen = viewport_tl +
                           (float(x) / displaySize.x) * viewport_v1 +
                           (float(y) / displaySize.y) * viewport_v2;

        glm::vec3 resultCol;
        bool is = scene->trace(Ray(screen, screen - eye), resultCol);

        if (is == true) {
          colBuff.setPixel(x, y, resultCol);
        } else {
          colBuff.setPixel(x, y, Colors::black);
        }
      }
    }
  }

  return colBuff;
}