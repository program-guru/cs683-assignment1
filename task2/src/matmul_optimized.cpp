// matmul_optimized.cpp  STAGE 3: PUT IT ALL TOGETHER
//
// This is the graded function AND the kernel that gets injected into llama.cpp. Combine
// everything you have learned across the whole assignment  loop reordering, register
// blocking and unrolling (Task 1 / Stage 1 here), cache tiling and software prefetch
// (Stage 2)  and TUNE it to be as fast as you can. Your speedup over matmul_naive determines
// your score (see the tier table the harness prints), and this same function will power a
// real LLM inference via `make llama-demo`.

// matmul_optimized.cpp

#include <immintrin.h>
#include <algorithm>
#include "matmul.h"

using namespace std;

static inline float hsum256_ps_avx(__m256 v) {
    __m128 vlow  = _mm256_castps256_ps128(v);
    __m128 vhigh = _mm256_extractf128_ps(v, 1);
    vlow  = _mm_add_ps(vlow, vhigh);
    __m128 shuf  = _mm_movehl_ps(vlow, vlow);
    vlow  = _mm_add_ps(vlow, shuf);
    shuf  = _mm_shuffle_ps(vlow, vlow, _MM_SHUFFLE(2, 3, 0, 1));
    vlow  = _mm_add_ps(vlow, shuf);
    return _mm_cvtss_f32(vlow);
}

void matmul_optimized(const float *A, const float *B, float *C,
                      int M, int N, int K, int lda, int ldb, int ldc) {
    const int BM = 96;
    const int BN = 64;

    for (int i0 = 0; i0 < M; i0 += BM) {
        int i_max = min(i0 + BM, M);
        
        for (int j0 = 0; j0 < N; j0 += BN) {
            int j_max = min(j0 + BN, N);
            
            int i = i0;

            //using 3x4 blocking

            for (; i <= i_max - 3; i += 3) {
                const float* a_row0 = A + (i + 0) * lda;
                const float* a_row1 = A + (i + 1) * lda;
                const float* a_row2 = A + (i + 2) * lda;

                int j = j0;
                for (; j <= j_max - 4; j += 4) {
                    const float* b_row0 = B + (j + 0) * ldb;
                    const float* b_row1 = B + (j + 1) * ldb;
                    const float* b_row2 = B + (j + 2) * ldb;
                    const float* b_row3 = B + (j + 3) * ldb;

                    __m256 c00 = _mm256_setzero_ps(), c01 = _mm256_setzero_ps(), c02 = _mm256_setzero_ps(), c03 = _mm256_setzero_ps();
                    __m256 c10 = _mm256_setzero_ps(), c11 = _mm256_setzero_ps(), c12 = _mm256_setzero_ps(), c13 = _mm256_setzero_ps();
                    __m256 c20 = _mm256_setzero_ps(), c21 = _mm256_setzero_ps(), c22 = _mm256_setzero_ps(), c23 = _mm256_setzero_ps();

                    int p = 0;
                    for (; p <= K - 8; p += 8) {
                        __builtin_prefetch(b_row0 + p + 128, 0, 1);

                        __m256 a0 = _mm256_loadu_ps(a_row0 + p);
                        __m256 a1 = _mm256_loadu_ps(a_row1 + p);
                        __m256 a2 = _mm256_loadu_ps(a_row2 + p);

                        __m256 b0 = _mm256_loadu_ps(b_row0 + p);
                        c00 = _mm256_fmadd_ps(a0, b0, c00); c10 = _mm256_fmadd_ps(a1, b0, c10); c20 = _mm256_fmadd_ps(a2, b0, c20);

                        __m256 b1 = _mm256_loadu_ps(b_row1 + p);
                        c01 = _mm256_fmadd_ps(a0, b1, c01); c11 = _mm256_fmadd_ps(a1, b1, c11); c21 = _mm256_fmadd_ps(a2, b1, c21);

                        __m256 b2 = _mm256_loadu_ps(b_row2 + p);
                        c02 = _mm256_fmadd_ps(a0, b2, c02); c12 = _mm256_fmadd_ps(a1, b2, c12); c22 = _mm256_fmadd_ps(a2, b2, c22);

                        __m256 b3 = _mm256_loadu_ps(b_row3 + p);
                        c03 = _mm256_fmadd_ps(a0, b3, c03); c13 = _mm256_fmadd_ps(a1, b3, c13); c23 = _mm256_fmadd_ps(a2, b3, c23);
                    }

                    float s00 = hsum256_ps_avx(c00), s01 = hsum256_ps_avx(c01), s02 = hsum256_ps_avx(c02), s03 = hsum256_ps_avx(c03);
                    float s10 = hsum256_ps_avx(c10), s11 = hsum256_ps_avx(c11), s12 = hsum256_ps_avx(c12), s13 = hsum256_ps_avx(c13);
                    float s20 = hsum256_ps_avx(c20), s21 = hsum256_ps_avx(c21), s22 = hsum256_ps_avx(c22), s23 = hsum256_ps_avx(c23);

                    for (; p < K; ++p) {
                        float a0 = a_row0[p], a1 = a_row1[p], a2 = a_row2[p];
                        float b0 = b_row0[p], b1 = b_row1[p], b2 = b_row2[p], b3 = b_row3[p];
                        
                        s00 += a0 * b0; s10 += a1 * b0; s20 += a2 * b0;
                        s01 += a0 * b1; s11 += a1 * b1; s21 += a2 * b1;
                        s02 += a0 * b2; s12 += a1 * b2; s22 += a2 * b2;
                        s03 += a0 * b3; s13 += a1 * b3; s23 += a2 * b3;
                    }

                    // Store C tile
                    float* c_row0 = C + (i + 0) * ldc + j;
                    float* c_row1 = C + (i + 1) * ldc + j;
                    float* c_row2 = C + (i + 2) * ldc + j;

                    c_row0[0] = s00; c_row0[1] = s01; c_row0[2] = s02; c_row0[3] = s03;
                    c_row1[0] = s10; c_row1[1] = s11; c_row1[2] = s12; c_row1[3] = s13;
                    c_row2[0] = s20; c_row2[1] = s21; c_row2[2] = s22; c_row2[3] = s23;
                }
            }

            //EDGE CASES
            int j_processed = j0 + ((j_max - j0) / 4) * 4; 
            for (int ii = i0; ii < i; ++ii) {
                const float* a_row = A + ii * lda;
                for (int jj = j_processed; jj < j_max; ++jj) {
                    const float* b_row = B + jj * ldb;
                    __m256 c0 = _mm256_setzero_ps();
                    int p = 0;
                    for (; p <= K - 8; p += 8) 
                        c0 = _mm256_fmadd_ps(_mm256_loadu_ps(a_row + p), _mm256_loadu_ps(b_row + p), c0);
                    float sum = hsum256_ps_avx(c0);
                    for (; p < K; ++p) sum += a_row[p] * b_row[p];
                    C[ii * ldc + jj] = sum;
                }
            }
            
            for (int ii = i; ii < i_max; ++ii) {
                const float* a_row = A + ii * lda;
                for (int jj = j0; jj < j_max; ++jj) {
                    const float* b_row = B + jj * ldb;
                    __m256 c0 = _mm256_setzero_ps();
                    int p = 0;
                    for (; p <= K - 8; p += 8) 
                        c0 = _mm256_fmadd_ps(_mm256_loadu_ps(a_row + p), _mm256_loadu_ps(b_row + p), c0);
                    float sum = hsum256_ps_avx(c0);
                    for (; p < K; ++p) sum += a_row[p] * b_row[p];
                    C[ii * ldc + jj] = sum;
                }
            }
        }
    }
    // TODO(student): replace this placeholder with your cache-blocked SIMD + prefetch
    // implementation.
    // matmul_naive(A, B, C, M, N, K, lda, ldb, ldc);
}