// matmul_simd.cpp  STAGE 1: SIMD with AVX2 intrinsics
#include "matmul.h"
#include <immintrin.h>
#include <iostream>
#include <vector>

void matmul_simd(const float *A, const float *B, float *C,
                 int M, int N, int K, int lda, int ldb, int ldc) {
    for (int i = 0; i < M; ++i) {
        for (int j = 0; j < N; ++j) {
            int dist = 8;
            __m256 sum_vec = _mm256_setzero_ps();
            int p = 0;

            for (; p <= K - dist; p += dist) {
                __m256 vec_a = _mm256_loadu_ps(&A[i * lda + p]);
                __m256 vec_b = _mm256_loadu_ps(&B[j * ldb + p]);
                sum_vec = _mm256_fmadd_ps(vec_a, vec_b, sum_vec);
            }

            float sum_arr[dist];
            float sum = 0;
            _mm256_storeu_ps(sum_arr, sum_vec);
            for (int q = 0; q < dist; q++) {
                sum += sum_arr[q];
            }

            for (; p < K; ++p) {
                sum += A[i * lda + p] * B[j * ldb + p];
            }

            C[i * ldc + j] = sum;
        }
    }
    // TODO(student): replace this placeholder with your register-tiled AVX2 implementation.
    // matmul_naive(A, B, C, M, N, K, lda, ldb, ldc);
}
