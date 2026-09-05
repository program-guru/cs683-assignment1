// matmul_prefetch.cpp  STAGE 2: CACHE BLOCKING + SOFTWARE PREFETCHING
#include <iostream>
#include <vector>
#include <immintrin.h>
#include "matmul.h"

using namespace std;

void matmul_prefetch(const float* A, const float* B, float* C,
                     int M, int N, int K, int lda, int ldb, int ldc) {
    const int m_block = 64;
    const int n_block = 64;
    const int p_dist = 512; 

    for(int i_blk = 0; i_blk < M; i_blk += m_block) {
        for(int j_blk = 0; j_blk < N; j_blk += n_block) {
            
            int i_max = min(i_blk + m_block, M);
            int j_max = min(j_blk + n_block, N);
            
            for(int i = i_blk; i < i_max; ++i) {
                for(int j = j_blk; j < j_max; ++j) {
                    
                    __m256 sum_vec = _mm256_setzero_ps();
                    int p = 0;
                    
                    for(; p <= K - 8; p += 8) {
                        __builtin_prefetch(&A[i * lda + p + p_dist], 0, 1);
                        __builtin_prefetch(&B[j * ldb + p + p_dist], 0, 1);
                        
                        __m256 vec_a = _mm256_loadu_ps(&A[i * lda + p]);
                        __m256 vec_b = _mm256_loadu_ps(&B[j * ldb + p]);
                        sum_vec = _mm256_fmadd_ps(vec_a, vec_b, sum_vec);
                    }
                    
                    float sum_arr[8];
                    _mm256_storeu_ps(sum_arr, sum_vec);
                    float sum = sum_arr[0] + sum_arr[1] + sum_arr[2] + sum_arr[3] + 
                                sum_arr[4] + sum_arr[5] + sum_arr[6] + sum_arr[7];
                    
                    for(; p < K; ++p) {
                        sum += A[i * lda + p] * B[j * ldb + p];
                    }
                    
                    C[i * ldc + j] = sum;
                }
            }
        }
    }
    // TODO(student): replace this placeholder with your cache-blocked SIMD + prefetch
    // implementation.
    // matmul_naive(A, B, C, M, N, K, lda, ldb, ldc);
}
