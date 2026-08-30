// conv_optimized.cpp  STAGE 5: PUT IT ALL TOGETHER
// Hint: measure after every change. Not every "optimization" helps  let the numbers,
// not intuition, decide.

#include <immintrin.h>

#include "convolution.h"

// void conv_optimized(const float* in, float* out, const float* ker,
//                     int H, int W, int K) {
    
//     conv_simd(in, out, ker, H, W, K);
// }



//------------------------------reorder+unroll------------------------------------------------


//void conv_optimized(const float* in, float* out, const float* ker,int H, int W, int K) {

 // TODO(student): replace this placeholder with your best combined implementation.
    // const int p = K / 2;
    // const int in_stride = W + 2 * p;

    // for (int i = 0; i < H * W; ++i) out[i] = 0.0f; 

    // for (int ky = 0; ky < K; ++ky) {
    //     for (int kx = 0; kx < K; ++kx) {
    //         const float w = ker[ky * K + kx];
    //         for (int oy = 0; oy < H; ++oy) {
    //             const int out_row = oy * W;
    //             const int in_row  = (oy + ky) * in_stride + kx;

    //             for (int ox = 0; ox < W; ox += 8) {
    //                 out[out_row + ox + 0] += w * in[in_row + ox + 0];
    //                 out[out_row + ox + 1] += w * in[in_row + ox + 1];
    //                 out[out_row + ox + 2] += w * in[in_row + ox + 2];
    //                 out[out_row + ox + 3] += w * in[in_row + ox + 3];
    //                 out[out_row + ox + 4] += w * in[in_row + ox + 4];
    //                 out[out_row + ox + 5] += w * in[in_row + ox + 5];
    //                 out[out_row + ox + 6] += w * in[in_row + ox + 6];
    //                 out[out_row + ox + 7] += w * in[in_row + ox + 7];
    //             }
    //         }
    //     }
    // }
//}


//--------------------------------tiling+unrolling---------------------------------------------


//void conv_optimized(const float* in, float* out, const float* ker,int H, int W, int K) {

    // const int p = K / 2;
    // const int in_stride = W + 2 * p;

    // const int TILE = 16;

    // if (K == 3) {

    //     const float k00 = ker[0];
    //     const float k01 = ker[1];
    //     const float k02 = ker[2];

    //     const float k10 = ker[3];
    //     const float k11 = ker[4];
    //     const float k12 = ker[5];

    //     const float k20 = ker[6];
    //     const float k21 = ker[7];
    //     const float k22 = ker[8];

    //     for (int oy0 = 0; oy0 < H; oy0 += TILE) {
    //         for (int ox0 = 0; ox0 < W; ox0 += TILE) {

    //             int oy_max = oy0 + TILE;
    //             if (oy_max > H) oy_max = H;

    //             int ox_max = ox0 + TILE;
    //             if (ox_max > W) ox_max = W;

    //             for (int oy = oy0; oy < oy_max; ++oy) {
    //                 for (int ox = ox0; ox < ox_max; ++ox) {

    //                     const int base = oy * in_stride + ox;

    //                     float acc = 0.0f;

    //                     acc += in[base] * k00;
    //                     acc += in[base + 1] * k01;
    //                     acc += in[base + 2] * k02;

    //                     acc += in[base + in_stride] * k10;
    //                     acc += in[base + in_stride + 1] * k11;
    //                     acc += in[base + in_stride + 2] * k12;

    //                     acc += in[base + 2 * in_stride] * k20;
    //                     acc += in[base + 2 * in_stride + 1] * k21;
    //                     acc += in[base + 2 * in_stride + 2] * k22;

    //                     out[oy * W + ox] = acc;
    //                 }
    //             }
    //         }
    //     }

    //     return;
    // }


    // // =========================
    // // K = 5 : Fully unrolled
    // // =========================
    // if (K == 5) {

    //     for (int oy0 = 0; oy0 < H; oy0 += TILE) {
    //         for (int ox0 = 0; ox0 < W; ox0 += TILE) {

    //             int oy_max = oy0 + TILE;
    //             if (oy_max > H) oy_max = H;

    //             int ox_max = ox0 + TILE;
    //             if (ox_max > W) ox_max = W;

    //             for (int oy = oy0; oy < oy_max; ++oy) {
    //                 for (int ox = ox0; ox < ox_max; ++ox) {

    //                     const int base = oy * in_stride + ox;

    //                     float acc = 0.0f;

    //                     // Row 0
    //                     acc += in[base] * ker[0];
    //                     acc += in[base + 1] * ker[1];
    //                     acc += in[base + 2] * ker[2];
    //                     acc += in[base + 3] * ker[3];
    //                     acc += in[base + 4] * ker[4];

    //                     // Row 1
    //                     acc += in[base + in_stride] * ker[5];
    //                     acc += in[base + in_stride + 1] * ker[6];
    //                     acc += in[base + in_stride + 2] * ker[7];
    //                     acc += in[base + in_stride + 3] * ker[8];
    //                     acc += in[base + in_stride + 4] * ker[9];

    //                     // Row 2
    //                     acc += in[base + 2 * in_stride] * ker[10];
    //                     acc += in[base + 2 * in_stride + 1] * ker[11];
    //                     acc += in[base + 2 * in_stride + 2] * ker[12];
    //                     acc += in[base + 2 * in_stride + 3] * ker[13];
    //                     acc += in[base + 2 * in_stride + 4] * ker[14];

    //                     // Row 3
    //                     acc += in[base + 3 * in_stride] * ker[15];
    //                     acc += in[base + 3 * in_stride + 1] * ker[16];
    //                     acc += in[base + 3 * in_stride + 2] * ker[17];
    //                     acc += in[base + 3 * in_stride + 3] * ker[18];
    //                     acc += in[base + 3 * in_stride + 4] * ker[19];

    //                     // Row 4
    //                     acc += in[base + 4 * in_stride] * ker[20];
    //                     acc += in[base + 4 * in_stride + 1] * ker[21];
    //                     acc += in[base + 4 * in_stride + 2] * ker[22];
    //                     acc += in[base + 4 * in_stride + 3] * ker[23];
    //                     acc += in[base + 4 * in_stride + 4] * ker[24];

    //                     out[oy * W + ox] = acc;
    //                 }
    //             }
    //         }
    //     }

    //     return;
    // }


    // // =========================
    // // Generic fallback
    // // =========================
    // for (int oy0 = 0; oy0 < H; oy0 += TILE) {
    //     for (int ox0 = 0; ox0 < W; ox0 += TILE) {

    //         int oy_max = oy0 + TILE;
    //         if (oy_max > H) oy_max = H;

    //         int ox_max = ox0 + TILE;
    //         if (ox_max > W) ox_max = W;

    //         for (int oy = oy0; oy < oy_max; ++oy) {
    //             for (int ox = ox0; ox < ox_max; ++ox) {

    //                 float acc = 0.0f;

    //                 for (int ky = 0; ky < K; ++ky) {
    //                     for (int kx = 0; kx < K; ++kx) {
    //                         acc +=
    //                             in[(oy + ky) * in_stride + ox + kx]
    //                             * ker[ky * K + kx];
    //                     }
    //                 }

    //                 out[oy * W + ox] = acc;
    //             }
    //         }
    //     }
    // }
//}


//-------------------simd+unrolling--------------------------------


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

            int ox = 0;

            // Process 8 output pixels at a time.
            for (; ox + 7 < W; ox += 8) {

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

            // Remaining pixels.
            for (; ox < W; ++ox) {

                float acc = 0.0f;

                acc += in[(oy + 0) * stride + ox + 0] * ker[0];
                acc += in[(oy + 0) * stride + ox + 1] * ker[1];
                acc += in[(oy + 0) * stride + ox + 2] * ker[2];

                acc += in[(oy + 1) * stride + ox + 0] * ker[3];
                acc += in[(oy + 1) * stride + ox + 1] * ker[4];
                acc += in[(oy + 1) * stride + ox + 2] * ker[5];

                acc += in[(oy + 2) * stride + ox + 0] * ker[6];
                acc += in[(oy + 2) * stride + ox + 1] * ker[7];
                acc += in[(oy + 2) * stride + ox + 2] * ker[8];

                out[oy * W + ox] = acc;
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

            int ox = 0;

            // Process 8 output pixels at a time.
            for (; ox + 7 < W; ox += 8) {

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

            // Scalar remainder.
            for (; ox < W; ++ox) {

                float acc = 0.0f;

                for (int ky = 0; ky < 5; ++ky) {
                    for (int kx = 0; kx < 5; ++kx) {
                        acc +=
                            in[(oy + ky) * stride + ox + kx]
                            * ker[ky * 5 + kx];
                    }
                }

                out[oy * W + ox] = acc;
            }
        }

        return;
    }

    for (int oy = 0; oy < H; ++oy) {

        int ox = 0;

        // SIMD path
        for (; ox + 7 < W; ox += 8) {

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

        // Scalar remainder
        for (; ox < W; ++ox) {

            float acc = 0.0f;

            for (int ky = 0; ky < K; ++ky) {
                for (int kx = 0; kx < K; ++kx) {

                    acc +=
                        in[(oy + ky) * stride + ox + kx]
                        * ker[ky * K + kx];
                }
            }

            out[oy * W + ox] = acc;
        }
    }
}



//---------------------------SIMD + unrolling + 2 accumulators + row pointers------------------------

// void conv_optimized(const float* __restrict in,
//                     float* __restrict out,
//                     const float* __restrict ker,
//                     int H, int W, int K)
// {
//     const int p = K / 2;
//     const int stride = W + 2 * p;


//     if (K == 3) {

//         // Broadcast kernel values once
//         const __m256 k0 = _mm256_set1_ps(ker[0]);
//         const __m256 k1 = _mm256_set1_ps(ker[1]);
//         const __m256 k2 = _mm256_set1_ps(ker[2]);
//         const __m256 k3 = _mm256_set1_ps(ker[3]);
//         const __m256 k4 = _mm256_set1_ps(ker[4]);
//         const __m256 k5 = _mm256_set1_ps(ker[5]);
//         const __m256 k6 = _mm256_set1_ps(ker[6]);
//         const __m256 k7 = _mm256_set1_ps(ker[7]);
//         const __m256 k8 = _mm256_set1_ps(ker[8]);

//         for (int oy = 0; oy < H; ++oy) {

//             // Precompute pointers to the three input rows
//             const float* row0 = in + (oy + 0) * stride;
//             const float* row1 = in + (oy + 1) * stride;
//             const float* row2 = in + (oy + 2) * stride;

//             float* out_row = out + oy * W;

//             int ox = 0;

//             // Process 16 output pixels per iteration
//             for (; ox + 15 < W; ox += 16) {

//                 // Two independent accumulators
//                 __m256 acc0 = _mm256_setzero_ps();
//                 __m256 acc1 = _mm256_setzero_ps();


//                 // =================================================
//                 // First 8 outputs: ox ... ox+7
//                 // =================================================

//                 acc0 = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row0 + ox + 0),
//                     k0, acc0);

//                 acc0 = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row0 + ox + 1),
//                     k1, acc0);

//                 acc0 = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row0 + ox + 2),
//                     k2, acc0);

//                 acc0 = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row1 + ox + 0),
//                     k3, acc0);

//                 acc0 = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row1 + ox + 1),
//                     k4, acc0);

//                 acc0 = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row1 + ox + 2),
//                     k5, acc0);

//                 acc0 = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row2 + ox + 0),
//                     k6, acc0);

//                 acc0 = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row2 + ox + 1),
//                     k7, acc0);

//                 acc0 = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row2 + ox + 2),
//                     k8, acc0);


//                 // =================================================
//                 // Second 8 outputs: ox+8 ... ox+15
//                 // =================================================

//                 acc1 = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row0 + ox + 8),
//                     k0, acc1);

//                 acc1 = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row0 + ox + 9),
//                     k1, acc1);

//                 acc1 = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row0 + ox + 10),
//                     k2, acc1);

//                 acc1 = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row1 + ox + 8),
//                     k3, acc1);

//                 acc1 = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row1 + ox + 9),
//                     k4, acc1);

//                 acc1 = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row1 + ox + 10),
//                     k5, acc1);

//                 acc1 = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row2 + ox + 8),
//                     k6, acc1);

//                 acc1 = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row2 + ox + 9),
//                     k7, acc1);

//                 acc1 = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row2 + ox + 10),
//                     k8, acc1);


//                 // Store 16 results
//                 _mm256_storeu_ps(out_row + ox, acc0);
//                 _mm256_storeu_ps(out_row + ox + 8, acc1);
//             }


//             for (; ox + 7 < W; ox += 8) {

//                 __m256 acc = _mm256_setzero_ps();

//                 acc = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row0 + ox + 0),
//                     k0, acc);

//                 acc = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row0 + ox + 1),
//                     k1, acc);

//                 acc = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row0 + ox + 2),
//                     k2, acc);

//                 acc = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row1 + ox + 0),
//                     k3, acc);

//                 acc = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row1 + ox + 1),
//                     k4, acc);

//                 acc = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row1 + ox + 2),
//                     k5, acc);

//                 acc = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row2 + ox + 0),
//                     k6, acc);

//                 acc = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row2 + ox + 1),
//                     k7, acc);

//                 acc = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row2 + ox + 2),
//                     k8, acc);

//                 _mm256_storeu_ps(out_row + ox, acc);
//             }

//             for (; ox < W; ++ox) {

//                 float acc = 0.0f;

//                 acc += row0[ox + 0] * ker[0];
//                 acc += row0[ox + 1] * ker[1];
//                 acc += row0[ox + 2] * ker[2];

//                 acc += row1[ox + 0] * ker[3];
//                 acc += row1[ox + 1] * ker[4];
//                 acc += row1[ox + 2] * ker[5];

//                 acc += row2[ox + 0] * ker[6];
//                 acc += row2[ox + 1] * ker[7];
//                 acc += row2[ox + 2] * ker[8];

//                 out_row[ox] = acc;
//             }
//         }

//         return;
//     }



//     if (K == 5) {

//         const __m256 k0  = _mm256_set1_ps(ker[0]);
//         const __m256 k1  = _mm256_set1_ps(ker[1]);
//         const __m256 k2  = _mm256_set1_ps(ker[2]);
//         const __m256 k3  = _mm256_set1_ps(ker[3]);
//         const __m256 k4  = _mm256_set1_ps(ker[4]);

//         const __m256 k5  = _mm256_set1_ps(ker[5]);
//         const __m256 k6  = _mm256_set1_ps(ker[6]);
//         const __m256 k7  = _mm256_set1_ps(ker[7]);
//         const __m256 k8  = _mm256_set1_ps(ker[8]);
//         const __m256 k9  = _mm256_set1_ps(ker[9]);

//         const __m256 k10 = _mm256_set1_ps(ker[10]);
//         const __m256 k11 = _mm256_set1_ps(ker[11]);
//         const __m256 k12 = _mm256_set1_ps(ker[12]);
//         const __m256 k13 = _mm256_set1_ps(ker[13]);
//         const __m256 k14 = _mm256_set1_ps(ker[14]);

//         const __m256 k15 = _mm256_set1_ps(ker[15]);
//         const __m256 k16 = _mm256_set1_ps(ker[16]);
//         const __m256 k17 = _mm256_set1_ps(ker[17]);
//         const __m256 k18 = _mm256_set1_ps(ker[18]);
//         const __m256 k19 = _mm256_set1_ps(ker[19]);

//         const __m256 k20 = _mm256_set1_ps(ker[20]);
//         const __m256 k21 = _mm256_set1_ps(ker[21]);
//         const __m256 k22 = _mm256_set1_ps(ker[22]);
//         const __m256 k23 = _mm256_set1_ps(ker[23]);
//         const __m256 k24 = _mm256_set1_ps(ker[24]);


//         for (int oy = 0; oy < H; ++oy) {

//             // Precompute row pointers
//             const float* row0 = in + (oy + 0) * stride;
//             const float* row1 = in + (oy + 1) * stride;
//             const float* row2 = in + (oy + 2) * stride;
//             const float* row3 = in + (oy + 3) * stride;
//             const float* row4 = in + (oy + 4) * stride;

//             float* out_row = out + oy * W;

//             int ox = 0;


//             // Process 16 output pixels at a time
//             for (; ox + 15 < W; ox += 16) {

//                 __m256 acc0 = _mm256_setzero_ps();
//                 __m256 acc1 = _mm256_setzero_ps();


//                 // =================================================
//                 // First 8 outputs
//                 // =================================================

//                 acc0 = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row0 + ox + 0), k0, acc0);
//                 acc0 = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row0 + ox + 1), k1, acc0);
//                 acc0 = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row0 + ox + 2), k2, acc0);
//                 acc0 = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row0 + ox + 3), k3, acc0);
//                 acc0 = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row0 + ox + 4), k4, acc0);

//                 acc0 = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row1 + ox + 0), k5, acc0);
//                 acc0 = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row1 + ox + 1), k6, acc0);
//                 acc0 = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row1 + ox + 2), k7, acc0);
//                 acc0 = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row1 + ox + 3), k8, acc0);
//                 acc0 = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row1 + ox + 4), k9, acc0);

//                 acc0 = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row2 + ox + 0), k10, acc0);
//                 acc0 = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row2 + ox + 1), k11, acc0);
//                 acc0 = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row2 + ox + 2), k12, acc0);
//                 acc0 = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row2 + ox + 3), k13, acc0);
//                 acc0 = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row2 + ox + 4), k14, acc0);

//                 acc0 = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row3 + ox + 0), k15, acc0);
//                 acc0 = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row3 + ox + 1), k16, acc0);
//                 acc0 = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row3 + ox + 2), k17, acc0);
//                 acc0 = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row3 + ox + 3), k18, acc0);
//                 acc0 = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row3 + ox + 4), k19, acc0);

//                 acc0 = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row4 + ox + 0), k20, acc0);
//                 acc0 = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row4 + ox + 1), k21, acc0);
//                 acc0 = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row4 + ox + 2), k22, acc0);
//                 acc0 = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row4 + ox + 3), k23, acc0);
//                 acc0 = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row4 + ox + 4), k24, acc0);


//                 // =================================================
//                 // Second 8 outputs
//                 // =================================================

//                 acc1 = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row0 + ox + 8), k0, acc1);
//                 acc1 = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row0 + ox + 9), k1, acc1);
//                 acc1 = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row0 + ox + 10), k2, acc1);
//                 acc1 = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row0 + ox + 11), k3, acc1);
//                 acc1 = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row0 + ox + 12), k4, acc1);

//                 acc1 = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row1 + ox + 8), k5, acc1);
//                 acc1 = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row1 + ox + 9), k6, acc1);
//                 acc1 = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row1 + ox + 10), k7, acc1);
//                 acc1 = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row1 + ox + 11), k8, acc1);
//                 acc1 = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row1 + ox + 12), k9, acc1);

//                 acc1 = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row2 + ox + 8), k10, acc1);
//                 acc1 = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row2 + ox + 9), k11, acc1);
//                 acc1 = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row2 + ox + 10), k12, acc1);
//                 acc1 = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row2 + ox + 11), k13, acc1);
//                 acc1 = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row2 + ox + 12), k14, acc1);

//                 acc1 = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row3 + ox + 8), k15, acc1);
//                 acc1 = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row3 + ox + 9), k16, acc1);
//                 acc1 = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row3 + ox + 10), k17, acc1);
//                 acc1 = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row3 + ox + 11), k18, acc1);
//                 acc1 = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row3 + ox + 12), k19, acc1);

//                 acc1 = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row4 + ox + 8), k20, acc1);
//                 acc1 = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row4 + ox + 9), k21, acc1);
//                 acc1 = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row4 + ox + 10), k22, acc1);
//                 acc1 = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row4 + ox + 11), k23, acc1);
//                 acc1 = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row4 + ox + 12), k24, acc1);


//                 _mm256_storeu_ps(out_row + ox, acc0);
//                 _mm256_storeu_ps(out_row + ox + 8, acc1);
//             }


//             // =====================================================
//             // Remaining 8-pixel SIMD block
//             // =====================================================

//             for (; ox + 7 < W; ox += 8) {

//                 __m256 acc = _mm256_setzero_ps();

//                 acc = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row0 + ox + 0), k0, acc);
//                 acc = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row0 + ox + 1), k1, acc);
//                 acc = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row0 + ox + 2), k2, acc);
//                 acc = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row0 + ox + 3), k3, acc);
//                 acc = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row0 + ox + 4), k4, acc);

//                 acc = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row1 + ox + 0), k5, acc);
//                 acc = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row1 + ox + 1), k6, acc);
//                 acc = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row1 + ox + 2), k7, acc);
//                 acc = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row1 + ox + 3), k8, acc);
//                 acc = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row1 + ox + 4), k9, acc);

//                 acc = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row2 + ox + 0), k10, acc);
//                 acc = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row2 + ox + 1), k11, acc);
//                 acc = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row2 + ox + 2), k12, acc);
//                 acc = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row2 + ox + 3), k13, acc);
//                 acc = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row2 + ox + 4), k14, acc);

//                 acc = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row3 + ox + 0), k15, acc);
//                 acc = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row3 + ox + 1), k16, acc);
//                 acc = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row3 + ox + 2), k17, acc);
//                 acc = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row3 + ox + 3), k18, acc);
//                 acc = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row3 + ox + 4), k19, acc);

//                 acc = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row4 + ox + 0), k20, acc);
//                 acc = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row4 + ox + 1), k21, acc);
//                 acc = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row4 + ox + 2), k22, acc);
//                 acc = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row4 + ox + 3), k23, acc);
//                 acc = _mm256_fmadd_ps(
//                     _mm256_loadu_ps(row4 + ox + 4), k24, acc);

//                 _mm256_storeu_ps(out_row + ox, acc);
//             }


//             // =====================================================
//             // Scalar remainder
//             // =====================================================

//             for (; ox < W; ++ox) {

//                 float acc = 0.0f;

//                 for (int ky = 0; ky < 5; ++ky) {
//                     for (int kx = 0; kx < 5; ++kx) {
//                         acc +=
//                             in[(oy + ky) * stride + ox + kx]
//                             * ker[ky * 5 + kx];
//                     }
//                 }

//                 out_row[ox] = acc;
//             }
//         }

//         return;
//     }


//     // ============================================================
//     // Generic K
//     // ============================================================

//     for (int oy = 0; oy < H; ++oy) {

//         const float* in_row = in + oy * stride;
//         float* out_row = out + oy * W;

//         int ox = 0;

//         for (; ox + 7 < W; ox += 8) {

//             __m256 acc = _mm256_setzero_ps();

//             for (int ky = 0; ky < K; ++ky) {

//                 const float* row = in_row + ky * stride;

//                 for (int kx = 0; kx < K; ++kx) {

//                     __m256 input =
//                         _mm256_loadu_ps(row + ox + kx);

//                     __m256 weight =
//                         _mm256_set1_ps(ker[ky * K + kx]);

//                     acc = _mm256_fmadd_ps(
//                         input, weight, acc);
//                 }
//             }

//             _mm256_storeu_ps(out_row + ox, acc);
//         }

//         // Scalar remainder
//         for (; ox < W; ++ox) {

//             float acc = 0.0f;

//             for (int ky = 0; ky < K; ++ky) {
//                 for (int kx = 0; kx < K; ++kx) {

//                     acc +=
//                         in[(oy + ky) * stride + ox + kx]
//                         * ker[ky * K + kx];
//                 }
//             }

//             out_row[ox] = acc;
//         }
//     }
// }