// conv_simd.cpp  STAGE 4: SIMD with AVX2 intrinsics
#include <immintrin.h>

#include "convolution.h"

void conv_simd(const float* in, float* out, const float* ker,
               int H, int W, int K) {
    const bool is_simd_256 = true;  // Set to true to use AVX2 (256-bit) intrinsics, false for SSE (128-bit)
    const int p = K / 2;
    const int in_stride = W + 2 * p;  

    for (int oy = 0; oy < H; ++oy) {
        for (int ox = 0; ox < W; ox += is_simd_256 ? 8 : 4) {  
            if(is_simd_256) {
                __m256 acc = _mm256_setzero_ps();
                for (int ky = 0; ky < K; ++ky) {
                    const float* row = in + (oy + ky) * in_stride + ox;
                    for (int kx = 0; kx < K; ++kx) {
                        // Load 8 consecutive input values
                        __m256 input = _mm256_loadu_ps(row + kx);
                        // Same kernel value is used for all 8 pixels
                        __m256 weight = _mm256_set1_ps(ker[ky * K + kx]);

                        acc = _mm256_add_ps(acc, _mm256_mul_ps(input, weight));
                    }
                }
                _mm256_storeu_ps(&out[oy * W + ox], acc);
            } else {
                __m128 acc = _mm_setzero_ps();
                for (int ky = 0; ky < K; ++ky) {
                    const float* row = in + (oy + ky) * in_stride + ox;
                    for (int kx = 0; kx < K; ++kx) {
                        // Load 8 consecutive input values
                        __m128 input = _mm_loadu_ps(row + kx);
                        // Same kernel value is used for all 8 pixels
                        __m128 weight = _mm_set1_ps(ker[ky * K + kx]);

                        acc = _mm_add_ps(acc, _mm_mul_ps(input, weight));
                    }
                }
                _mm_storeu_ps(&out[oy * W + ox], acc);
            }
        }
    }
}
