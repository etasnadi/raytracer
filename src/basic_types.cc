#include "basic_types.h"

#include "cuda_runtime.h"

#include "math.h"

namespace raytracer_cu {

template <class T>
ColorBuffer<T>::ColorBuffer(int a_w, int a_h) : w(a_w), h(a_h) {
  c = new T[w * h];
}

template <class T> int ColorBuffer<T>::idx(int x, int y) { return y * w + x; }

template <class T> void ColorBuffer<T>::setPixel(int x, int y, T intensity) {
  c[idx(x, y)] = intensity;
}

template <class T> T ColorBuffer<T>::getPixel(int x, int y, bool normalize) {
  T color = c[idx(x, y)];
  if (normalize) {
    return (color - minIntensity) / (color + maxIntensity);
  } else {
    return color;
  }
}

template <class T> T *ColorBuffer<T>::getRawPtr() {
  return c;
}

template <class T> ColorBuffer<T>::~ColorBuffer() {
  delete c;
}

template class ColorBuffer<float3>;
template class ColorBuffer<uint8_t>;
} // namespace raytracer_cu
