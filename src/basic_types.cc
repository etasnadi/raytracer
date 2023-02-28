#include "basic_types.h"

#include <glm/glm.hpp>

namespace raytracer {

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

  template class ColorBuffer<glm::vec3>;
}
