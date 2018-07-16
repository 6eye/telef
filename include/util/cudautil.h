#pragma once

#include <cstdio>
#include <cstdlib>

#ifdef DEBUG
#define CUDA_CHECK(errarg)   __checkErrorFunc(errarg, __FILE__, __LINE__)
#define CHECK_ERROR_MSG(errstr) __checkErrMsgFunc(errstr, __FILE__, __LINE__)
#else
#define CUDA_CHECK(arg)   arg
#define CHECK_ERROR_MSG(str) do {} while (0)
#endif

inline void __checkErrorFunc(cudaError_t errarg, const char* file,
                             const int line)
{
    if(errarg) {
        fprintf(stderr, "Error \"%s\" at %s(%i)\n", cudaGetErrorName(errarg), file, line);
        exit(EXIT_FAILURE);
    }
}


inline void __checkErrMsgFunc(const char* errstr, const char* file,
                              const int line)
{
    cudaError_t err = cudaGetLastError();
    if(err != cudaSuccess) {
        fprintf(stderr, "Error: %s at %s(%i): %s\n",
                errstr, file, line, cudaGetErrorString(err));
        exit(EXIT_FAILURE);
    }
}

