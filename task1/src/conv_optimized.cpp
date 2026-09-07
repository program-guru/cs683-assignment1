// conv_optimized.cpp  STAGE 5: PUT IT ALL TOGETHER
// Hint: measure after every change. Not every "optimization" helps  let the numbers,
// not intuition, decide.

#include <immintrin.h>

#include "convolution.h"

//--------------------------------simd+unrolling---------------------------------------------

void conv_optimized(const float* in, float* out, const float* ker,
                    int H, int W, int K)
{
    const int p = K / 2;
    const int stride = W + 2 * p;

    if (K == 3) {

        // Broadcast kernel values once.
        __m256 k0 = _mm256_set1_ps(ker[0]);
        __m256 k1 = _mm256_set1_ps(ker[1]);
        __m256 k2 = _mm256_set1_ps(ker[2]);
        __m256 k3 = _mm256_set1_ps(ker[3]);
        __m256 k4 = _mm256_set1_ps(ker[4]);
        __m256 k5 = _mm256_set1_ps(ker[5]);
        __m256 k6 = _mm256_set1_ps(ker[6]);
        __m256 k7 = _mm256_set1_ps(ker[7]);
        __m256 k8 = _mm256_set1_ps(ker[8]);

        for (int oy = 0; oy < H; ++oy) {
            // Process 8 output pixels at a time.
            for (int ox = 0; ox + 7 < W; ox += 8) {

                __m256 acc = _mm256_setzero_ps();

                // Row 0
                acc = _mm256_fmadd_ps(
                    _mm256_loadu_ps(&in[(oy + 0) * stride + ox + 0]),
                    k0, acc);

                acc = _mm256_fmadd_ps(
                    _mm256_loadu_ps(&in[(oy + 0) * stride + ox + 1]),
                    k1, acc);

                acc = _mm256_fmadd_ps(
                    _mm256_loadu_ps(&in[(oy + 0) * stride + ox + 2]),
                    k2, acc);

                // Row 1
                acc = _mm256_fmadd_ps(
                    _mm256_loadu_ps(&in[(oy + 1) * stride + ox + 0]),
                    k3, acc);

                acc = _mm256_fmadd_ps(
                    _mm256_loadu_ps(&in[(oy + 1) * stride + ox + 1]),
                    k4, acc);

                acc = _mm256_fmadd_ps(
                    _mm256_loadu_ps(&in[(oy + 1) * stride + ox + 2]),
                    k5, acc);

                // Row 2
                acc = _mm256_fmadd_ps(
                    _mm256_loadu_ps(&in[(oy + 2) * stride + ox + 0]),
                    k6, acc);

                acc = _mm256_fmadd_ps(
                    _mm256_loadu_ps(&in[(oy + 2) * stride + ox + 1]),
                    k7, acc);

                acc = _mm256_fmadd_ps(
                    _mm256_loadu_ps(&in[(oy + 2) * stride + ox + 2]),
                    k8, acc);

                _mm256_storeu_ps(&out[oy * W + ox], acc);
            }
        }

        return;
    }


    if (K == 5) {

        // Broadcast kernel values once.
        __m256 k0  = _mm256_set1_ps(ker[0]);
        __m256 k1  = _mm256_set1_ps(ker[1]);
        __m256 k2  = _mm256_set1_ps(ker[2]);
        __m256 k3  = _mm256_set1_ps(ker[3]);
        __m256 k4  = _mm256_set1_ps(ker[4]);

        __m256 k5  = _mm256_set1_ps(ker[5]);
        __m256 k6  = _mm256_set1_ps(ker[6]);
        __m256 k7  = _mm256_set1_ps(ker[7]);
        __m256 k8  = _mm256_set1_ps(ker[8]);
        __m256 k9  = _mm256_set1_ps(ker[9]);

        __m256 k10 = _mm256_set1_ps(ker[10]);
        __m256 k11 = _mm256_set1_ps(ker[11]);
        __m256 k12 = _mm256_set1_ps(ker[12]);
        __m256 k13 = _mm256_set1_ps(ker[13]);
        __m256 k14 = _mm256_set1_ps(ker[14]);

        __m256 k15 = _mm256_set1_ps(ker[15]);
        __m256 k16 = _mm256_set1_ps(ker[16]);
        __m256 k17 = _mm256_set1_ps(ker[17]);
        __m256 k18 = _mm256_set1_ps(ker[18]);
        __m256 k19 = _mm256_set1_ps(ker[19]);

        __m256 k20 = _mm256_set1_ps(ker[20]);
        __m256 k21 = _mm256_set1_ps(ker[21]);
        __m256 k22 = _mm256_set1_ps(ker[22]);
        __m256 k23 = _mm256_set1_ps(ker[23]);
        __m256 k24 = _mm256_set1_ps(ker[24]);


        for (int oy = 0; oy < H; ++oy) {
            // Process 8 output pixels at a time.
            for (int ox = 0; ox + 7 < W; ox += 8) {

                __m256 acc = _mm256_setzero_ps();

                // Row 0
                acc = _mm256_fmadd_ps(
                    _mm256_loadu_ps(&in[(oy + 0) * stride + ox + 0]),
                    k0, acc);
                acc = _mm256_fmadd_ps(
                    _mm256_loadu_ps(&in[(oy + 0) * stride + ox + 1]),
                    k1, acc);
                acc = _mm256_fmadd_ps(
                    _mm256_loadu_ps(&in[(oy + 0) * stride + ox + 2]),
                    k2, acc);
                acc = _mm256_fmadd_ps(
                    _mm256_loadu_ps(&in[(oy + 0) * stride + ox + 3]),
                    k3, acc);
                acc = _mm256_fmadd_ps(
                    _mm256_loadu_ps(&in[(oy + 0) * stride + ox + 4]),
                    k4, acc);

                // Row 1
                acc = _mm256_fmadd_ps(
                    _mm256_loadu_ps(&in[(oy + 1) * stride + ox + 0]),
                    k5, acc);
                acc = _mm256_fmadd_ps(
                    _mm256_loadu_ps(&in[(oy + 1) * stride + ox + 1]),
                    k6, acc);
                acc = _mm256_fmadd_ps(
                    _mm256_loadu_ps(&in[(oy + 1) * stride + ox + 2]),
                    k7, acc);
                acc = _mm256_fmadd_ps(
                    _mm256_loadu_ps(&in[(oy + 1) * stride + ox + 3]),
                    k8, acc);
                acc = _mm256_fmadd_ps(
                    _mm256_loadu_ps(&in[(oy + 1) * stride + ox + 4]),
                    k9, acc);

                // Row 2
                acc = _mm256_fmadd_ps(
                    _mm256_loadu_ps(&in[(oy + 2) * stride + ox + 0]),
                    k10, acc);
                acc = _mm256_fmadd_ps(
                    _mm256_loadu_ps(&in[(oy + 2) * stride + ox + 1]),
                    k11, acc);
                acc = _mm256_fmadd_ps(
                    _mm256_loadu_ps(&in[(oy + 2) * stride + ox + 2]),
                    k12, acc);
                acc = _mm256_fmadd_ps(
                    _mm256_loadu_ps(&in[(oy + 2) * stride + ox + 3]),
                    k13, acc);
                acc = _mm256_fmadd_ps(
                    _mm256_loadu_ps(&in[(oy + 2) * stride + ox + 4]),
                    k14, acc);

                // Row 3
                acc = _mm256_fmadd_ps(
                    _mm256_loadu_ps(&in[(oy + 3) * stride + ox + 0]),
                    k15, acc);
                acc = _mm256_fmadd_ps(
                    _mm256_loadu_ps(&in[(oy + 3) * stride + ox + 1]),
                    k16, acc);
                acc = _mm256_fmadd_ps(
                    _mm256_loadu_ps(&in[(oy + 3) * stride + ox + 2]),
                    k17, acc);
                acc = _mm256_fmadd_ps(
                    _mm256_loadu_ps(&in[(oy + 3) * stride + ox + 3]),
                    k18, acc);
                acc = _mm256_fmadd_ps(
                    _mm256_loadu_ps(&in[(oy + 3) * stride + ox + 4]),
                    k19, acc);

                // Row 4
                acc = _mm256_fmadd_ps(
                    _mm256_loadu_ps(&in[(oy + 4) * stride + ox + 0]),
                    k20, acc);
                acc = _mm256_fmadd_ps(
                    _mm256_loadu_ps(&in[(oy + 4) * stride + ox + 1]),
                    k21, acc);
                acc = _mm256_fmadd_ps(
                    _mm256_loadu_ps(&in[(oy + 4) * stride + ox + 2]),
                    k22, acc);
                acc = _mm256_fmadd_ps(
                    _mm256_loadu_ps(&in[(oy + 4) * stride + ox + 3]),
                    k23, acc);
                acc = _mm256_fmadd_ps(
                    _mm256_loadu_ps(&in[(oy + 4) * stride + ox + 4]),
                    k24, acc);

                _mm256_storeu_ps(&out[oy * W + ox], acc);
            }
        }

        return;
    }

    for (int oy = 0; oy < H; ++oy) {
        // SIMD path
        for (int ox = 0; ox + 7 < W; ox += 8) {

            __m256 acc = _mm256_setzero_ps();

            for (int ky = 0; ky < K; ++ky) {
                for (int kx = 0; kx < K; ++kx) {

                    __m256 input = _mm256_loadu_ps(
                        &in[(oy + ky) * stride + ox + kx]
                    );

                    __m256 weight = _mm256_set1_ps(
                        ker[ky * K + kx]
                    );

                    acc = _mm256_fmadd_ps(
                        input,
                        weight,
                        acc
                    );
                }
            }

            _mm256_storeu_ps(
                &out[oy * W + ox],
                acc
            );
        }
    }
}
