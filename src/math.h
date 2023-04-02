#ifndef MATH_H
#define MATH_H

#include <ostream>

#include "cudastuff.h"
#include "cuda_runtime.h"

namespace raytracer_cu {

CUDA_HOSTDEV void printv(float3& a);

CUDA_HOSTDEV float sq(float x);
CUDA_HOSTDEV float abs(float x);
CUDA_HOSTDEV float sqrt(float x);

template <int N, int M> class Mat {
public:
  float data[N][M] = {0};
  CUDA_HOSTDEV Mat() {}
};
typedef Mat<3, 3> mat3x3;

CUDA_HOSTDEV void printm(mat3x3& a);

template <int N, int M, int K>
CUDA_HOSTDEV Mat<N, K> mm(Mat<N, M> a, Mat<M, K> b);
template <int N, int M> CUDA_HOSTDEV Mat<N, M> matadd(Mat<N, M> a, Mat<N, M> b);
template <int N, int M> CUDA_HOSTDEV Mat<N, M> mm(Mat<N, M> a, float s);
template <int N, int M> CUDA_HOSTDEV Mat<N, M> mm(float s, Mat<N, M> a);
template <int N> CUDA_HOSTDEV float3 mm(mat3x3 a, float3 v);
template <int N> CUDA_HOSTDEV float3 mm(float3 v, mat3x3 a);
template <int N> CUDA_HOSTDEV Mat<N, N> eye();
template <int N, int M> CUDA_HOSTDEV Mat<N, M> zeros();

CUDA_HOSTDEV float dot(float3 a, float3 b);
CUDA_HOSTDEV float3 cross(float3 a, float3 b);
CUDA_HOSTDEV float3 operator+(float3 a, float3 b);
CUDA_HOSTDEV float3 operator-(float3 a, float3 b);
CUDA_HOSTDEV float3 operator*(float3 a, float3 b);
CUDA_HOSTDEV float3 operator*(float a, float3 b);
CUDA_HOSTDEV float3 operator*(float3 a, float b);
//CUDA_HOSTDEV float3 operator/(float3 a, float b);
CUDA_HOSTDEV float3 div(float3 a, float b);
CUDA_HOSTDEV float3 operator/(float3 a, float3 b);
CUDA_HOSTDEV float length(float3 a);
CUDA_HOSTDEV float3 norm(float3 a);
CUDA_HOST std::ostream &operator<<(std::ostream &os, float3 x);

CUDA_HOSTDEV float2 operator+(float2 a, float2 b);
CUDA_HOSTDEV float2 operator-(float2 a, float2 b);
CUDA_HOSTDEV float2 operator*(float2 a, float2 b);
CUDA_HOSTDEV float2 operator*(float a, float2 b);
CUDA_HOSTDEV float2 operator*(float2 a, float b);
CUDA_HOSTDEV float2 operator/(float2 a, float b);

} // namespace raytracer_cu

#endif