#include "math.h"

#include <cmath>
#include <ostream>

#include "cuda_runtime.h"

namespace raytracer_cu {

float sq(float x) { return x * x; }

float abs(float x) {
#ifdef __CUDA_ARCH__
  return fabsf(x);
#else
  return std::abs(x);
#endif
}

float sqrt(float x) {
#ifdef __CUDA_ARCH__
  return sqrtf(x);
#else
  return std::sqrt(x);
#endif
}

void printv(float3& a){
  printf("(%.2f %.2f %.2f) ", a.x, a.y, a.z);
}

void printm(mat3x3& a){
  printf("Matrix:\n");
  for(int i = 0; i < 3; i++){
    for(int j = 0; j < 3; j++){
      printf("%.2f ", a.data[i][j]);
    }
    printf("\n");
  }
}

// =======================================================
// Matrix functions
// =======================================================

/*

A, B: matrices
s:    scalar
v:    vector (float3)

C=mm(A,B): C=A*B, *: matrix multiplication
C=A+B
B=mm(A,s): B=A*s
B=mm(s,A): B=s*A
B=mm(A,v): B=A*v
B=mm(v,A): B=v*A
A=eye() for sqared matrices
A=zeros() any shape

*/

template <int N, int M, int K>
Mat<N, K> mm(Mat<N, M> a, Mat<M, K> b) {
  Mat<N, K> c;
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < K; j++) {
      float dot = 0.0f;
      for (int k = 0; k < M; k++) {
        dot += a.data[i][k] * b.data[k][j];
      }
      c.data[i][j] = dot;
    }
  }
  return c;
}

template <int N, int M>
Mat<N, M> matadd(Mat<N, M> a, Mat<N, M> b) {
  Mat<N, M> c;
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < M; j++) {
      c[i][j] = a[i][j] + b[i][j];
    }
  }
  return c;
}

template <int N, int M> Mat<N, M> mm(Mat<N, M> a, float s) {
  Mat<N, M> c;
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < M; j++) {
      c[i][j] = a[i][j] * s;
    }
  }
  return c;
}

template <int N, int M> Mat<N, M> mm(float s, Mat<N, M> a) {
  return mm(a, s);
}

template <int N> float3 mm(mat3x3 a, float3 v) {
  Mat<3, 1> matVec;
  matVec.data[0][0] = v.x;
  matVec.data[1][0] = v.y;
  matVec.data[2][0] = v.z;
  Mat<3, 1> result = mm(a, matVec);
  return make_float3(result.data[0][0], result.data[1][0], result.data[2][0]);
}

template <int N> float3 mm(float3 v, mat3x3 a) {
  Mat<1, 3> matVec;
  matVec.data[0][0] = v.x;
  matVec.data[0][1] = v.y;
  matVec.data[0][2] = v.z;
  Mat<1, 3> result = mm(matVec, a);
  return make_float3(result.data[0][0], result.data[0][1], result.data[0][2]);
}

template <int N> Mat<N, N> eye() {
  Mat<N, N> result;
  for (int i = 0; i < N; i++) {
    result.data[i][i] = 1;
  }
  return result;
}

template <int N, int M> Mat<N, M> zeros() { return Mat<N, M>(); }

template mat3x3 eye<3>();
template float3 mm<3>(mat3x3, float3);
template mat3x3 mm<3, 3, 3>(mat3x3, mat3x3);
template mat3x3 zeros<3, 3>();

// =======================================================
// float3 overloads
// =======================================================

/*
v, v1, v2:  float3
s:          scalar

s=dot(v1, v2): v=v1.v2
v=cross(v1, v2) v=v1xv2
  see: https://registry.khronos.org/OpenGL-Refpages/gl4/html/cross.xhtml
v=v1+v2
v=v1-v2
v=v1*v2 (elementwise multiplication)
v'=s*v
v'=v*s
v'=v/s
v'=length(v): v'=|v| (L2 of V)
v'=norm(v): v'=v/|v|

*/

float dot(float3 a, float3 b) {
  return a.x * b.x + a.y * b.y + a.z * b.z;
}

float3 cross(float3 a, float3 b) {
  return make_float3(a.y * b.z - b.y * a.z, a.z * b.x - b.z * a.x,
                     a.x * b.y - b.x * a.y);
}

float3 operator+(float3 a, float3 b) {
  return make_float3(a.x + b.x, a.y + b.y, a.z + b.z);
}

float3 operator-(float3 a, float3 b) {
  return make_float3(a.x - b.x, a.y - b.y, a.z - b.z);
}

float3 operator*(float3 a, float3 b) {
  return make_float3(a.x * b.x, a.y * b.y, a.z * b.z);
}

float3 operator*(float a, float3 b) {
  return make_float3(a * b.x, a * b.y, a * b.z);
}

float3 operator*(float3 a, float b) {
  return make_float3(a.x * b, a.y * b, a.z * b);
}

float3 operator/(float3 a, float b) {
  return make_float3(a.x/b, a.y/b, a.z/b);
}

float3 div(float3 a, float b){
  return make_float3(a.x/b, a.y/b, a.z/b);
}

float3 operator/(float3 a, float3 b) {
  return make_float3(a.x / b.x, a.y / b.y, a.z / b.z);
}

float2 operator-(float2 a, float2 b){
  return make_float2(a.x - b.x, a.y - b.x);
}

float2 operator+(float2 a, float2 b){
  return make_float2(a.x + b.x, a.y + b.y);
}

float2 operator*(float2 a, float b){
  return make_float2(a.x*b, a.y*b);
}

float2 operator*(float a, float2 b){
  return make_float2(a*b.x, a*b.y);
}

float length(float3 a) {
#ifdef __CUDA_ARCH__
  return norm3df(a.x, a.y, a.z);
#else
  return std::sqrt(sq(a.x) + sq(a.y) + sq(a.z));
#endif
}

float3 norm(float3 a) {
#ifdef __CUDA_ARCH__
  float invLen = rnorm3df(a.x, a.y, a.z);
  return a * invLen;
#else
  float len = std::sqrt(sq(a.x) + sq(a.y) + sq(a.z));
  return div(a, len);
#endif
}

std::ostream &operator<<(std::ostream &os, float3 v) {
  os << v.x << "," << v.y << ","
     << "v.z";
  return os;
}

// =======================================================

} // namespace raytracer_cu