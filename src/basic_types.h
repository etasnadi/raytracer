#ifndef BASIC_TYPES_H
#define BASIC_TYPES_H

#include <vector>

#define CUDA_TYPES
// #define DEBUG_STDOUT

namespace raytracer {

  template <class T> class ColorBuffer {
  private:
    T maxIntensity;
    T minIntensity;

  public:
    std::vector<T> c;
    const int w, h;
    ColorBuffer(int w, int h);
    void setPixel(int x, int y, T intensity);
    T getPixel(int x, int y, bool normalize = false);
    int idx(int x, int y);
  };

}

#endif
