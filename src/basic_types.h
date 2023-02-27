#ifndef BASIC_TYPES_H
#define BASIC_TYPES_H

#include <vector>

template <class T> class ColorBuffer {
private:
  std::vector<T> c;
  T maxIntensity;
  T minIntensity;

public:
  const int w, h;
  ColorBuffer(int w, int h);
  void setPixel(int x, int y, T intensity);
  T getPixel(int x, int y, bool normalize = false);
  int idx(int x, int y);
};

#endif