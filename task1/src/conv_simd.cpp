// conv_simd.cpp  STAGE 4: SIMD with AVX2 intrinsics
#include <immintrin.h>

#include "convolution.h"

void conv_simd(const float* in, float* out, const float* ker,
               int H, int W, int K) {
    // TODO(student): replace this placeholder with your AVX2 implementation.
    const int p = K / 2;
    const int in_stride = W + 2 * p;
    for (int oy = 0; oy < H; ++oy) {
        int ox = 0;
        for (; ox + 7 < W; ox += 8) {
            __m256 acc = _mm256_setzero_ps();
            for (int ky = 0; ky < K; ++ky) {
                for (int kx = 0; kx < K; ++kx) {
                    // Load 8 consecutive input values
                    __m256 input = _mm256_loadu_ps(&in[(oy + ky) * in_stride + (ox + kx)]);
                    // Same kernel value is used for all 8 pixels
                    __m256 weight = _mm256_set1_ps(ker[ky * K + kx]);
                    // acc += input * weight
                    acc = _mm256_add_ps(acc, _mm256_mul_ps(input, weight));
                }
            }
            // Store 8 output values
            _mm256_storeu_ps(&out[oy * W + ox], acc);
        }
        // Handle remaining pixels
        for (; ox < W; ++ox) {

            float acc = 0.0f;

            for (int ky = 0; ky < K; ++ky) {
                for (int kx = 0; kx < K; ++kx) {

                    acc +=
                        in[(oy + ky) * in_stride + (ox + kx)]
                        * ker[ky * K + kx];
                }
            }

            out[oy * W + ox] = acc;
        }
    }
}
