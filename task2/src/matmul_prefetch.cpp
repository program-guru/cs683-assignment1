// matmul_prefetch.cpp  STAGE 2: CACHE BLOCKING + SOFTWARE PREFETCHING

#include <immintrin.h>

#include "matmul.h"

void matmul_prefetch(const float* A, const float* B, float* C,
                     int M, int N, int K, int lda, int ldb, int ldc) {
    int p_dist = 64;
    for(int i = 0; i < M; ++i) {
        for(int j = 0; j < N; ++j) {
            float sum = 0.0f;
            int p = 0;
            
            for(; p < K - p_dist; p += p_dist) {
                __builtin_prefetch(&A[i * lda + p + p_dist], 0, 1);
                __builtin_prefetch(&B[j * ldb + p + p_dist], 0, 1);
                
                for(int p_inner = 0; p_inner < p_dist; ++p_inner) {
                    sum += A[i * lda + p + p_inner] * B[j * ldb + p + p_inner];
                }
            }
            
            for(; p < K; ++p) {
                sum += A[i * lda + p] * B[j * ldb + p];
            }
            
            C[i * ldc + j] = sum;
        }
    }
    // TODO(student): replace this placeholder with your cache-blocked SIMD + prefetch
    // implementation.
    matmul_naive(A, B, C, M, N, K, lda, ldb, ldc);
}
