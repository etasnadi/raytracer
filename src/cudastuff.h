#ifdef __CUDACC__
    // https://stackoverflow.com/questions/32014839/how-to-use-a-cuda-class-header-file-in-both-cpp-and-cuda-modules
    #define CUDA_HOSTDEV __host__ __device__
    #define CUDA_HOST __host__
    #define CUDA_DEVICE __device__
    #define CUDA_GLOBAL __global__
#else
    #define CUDA_HOSTDEV
    #define CUDA_HOST
    #define CUDA_DEVICE
    #define CUDA_GLOBAL
#endif