#ifndef BASIC_TYPES_H
#define BASIC_TYPES_H

#include <cstdint>
#include <stdexcept>
#include <vector>

#include "cudastuff.h"

#define CUDA_TYPES
// #define DEBUG_STDOUT
#define RAYTRACER_NAMESPACE raytracer_cu


namespace raytracer_cu {

template <class T> class ColorBuffer {
private:
  T maxIntensity;
  T minIntensity;

public:
  T *c;
  const int w, h;
  CUDA_HOSTDEV ColorBuffer(int w, int h);
  CUDA_HOSTDEV void setPixel(int x, int y, T intensity);
  CUDA_HOSTDEV T getPixel(int x, int y, bool normalize = false);
  CUDA_HOSTDEV int idx(int x, int y);
  CUDA_HOSTDEV T *getRawPtr();
  CUDA_HOSTDEV ~ColorBuffer();
};
template <class T, class SizeType = uint64_t> class EasyVector {
private:
  T *data;
  SizeType capacity;
  SizeType tailIdx = 0;

public:
  CUDA_HOSTDEV EasyVector() : capacity(12), data(new T[12]) {}
  CUDA_HOSTDEV EasyVector(SizeType length) {
    capacity = length;
    data = new T[length];
  }
  CUDA_HOSTDEV void allocate(SizeType n) {
    data = new T[n];
    capacity = n;
  }
  CUDA_HOSTDEV int getCapacity() { return capacity; }
  CUDA_HOSTDEV SizeType size() const { return tailIdx; }
  CUDA_HOSTDEV T &operator[](SizeType index) {
    if (index >= tailIdx) {
      // throw std::invalid_argument("Vector index out of bounds!");
    }
    return data[index];
  }
  CUDA_HOSTDEV T operator[](SizeType index) const {
    if (index >= tailIdx) {
      // throw std::invalid_argument("Vector index out of bounds!");
    }
    return data[index];
  }
  CUDA_HOSTDEV void push_back(T elem) {
    if (tailIdx >= capacity) {
      grow();
    }

    data[tailIdx] = elem;
    tailIdx += 1;
  }

  CUDA_HOSTDEV void grow() {
    SizeType newCapacity = capacity * 2;
    T *newData = new T[newCapacity];
    for (int i = 0; i < capacity; i++) {
      newData[i] = data[i];
    }
    capacity = newCapacity;
    data = newData;
  }
};

} // namespace raytracer_cu

#endif
