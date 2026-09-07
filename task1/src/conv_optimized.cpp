// conv_optimized.cpp  STAGE 5: PUT IT ALL TOGETHER
// Hint: measure after every change. Not every "optimization" helps  let the numbers,
// not intuition, decide.

#include <immintrin.h>

#include "convolution.h"

void implementation1(const float *in, float *out, const float *ker,
                     int H, int W, int K);
void implementation2(const float *in, float *out, const float *ker,
                     int H, int W, int K);
void implementation3(const float *in, float *out, const float *ker,
                     int H, int W, int K);
void implementation4(const float *in, float *out, const float *ker,
                     int H, int W, int K);
void implementation5(const float *in, float *out, const float *ker,
                     int H, int W, int K);

void conv_optimized(const float *in, float *out, const float *ker,
                    int H, int W, int K) {

    implementation5(in, out, ker, H, W, K);
}

//------------------------------reorder+unroll------------------------------------------------

void implementation1(const float *in, float *out, const float *ker, int H, int W, int K) {
    const int p = K / 2;
    const int in_stride = W + 2 * p;

    for (int i = 0; i < H * W; ++i)
        out[i] = 0.0f;

    for (int ky = 0; ky < K; ++ky) {
        for (int kx = 0; kx < K; ++kx) {
            const float w = ker[ky * K + kx];
            for (int oy = 0; oy < H; ++oy) {
                const int out_row = oy * W;
                const int in_row = (oy + ky) * in_stride + kx;

                for (int ox = 0; ox < W; ox += 8) {
                    out[out_row + ox + 0] += w * in[in_row + ox + 0];
                    out[out_row + ox + 1] += w * in[in_row + ox + 1];
                    out[out_row + ox + 2] += w * in[in_row + ox + 2];
                    out[out_row + ox + 3] += w * in[in_row + ox + 3];
                    out[out_row + ox + 4] += w * in[in_row + ox + 4];
                    out[out_row + ox + 5] += w * in[in_row + ox + 5];
                    out[out_row + ox + 6] += w * in[in_row + ox + 6];
                    out[out_row + ox + 7] += w * in[in_row + ox + 7];
                }
            }
        }
    }
}

//--------------------------------tiling+unrolling---------------------------------------------

void implementation2(const float *in, float *out, const float *ker, int H, int W, int K) {
    const int p = K / 2;
    const int in_stride = W + 2 * p;
    const int tile_size = 64;

    if (K == 3) {

        const float k00 = ker[0];
        const float k01 = ker[1];
        const float k02 = ker[2];

        const float k10 = ker[3];
        const float k11 = ker[4];
        const float k12 = ker[5];

        const float k20 = ker[6];
        const float k21 = ker[7];
        const float k22 = ker[8];

        for (int oy0 = 0; oy0 < H; oy0 += tile_size) {
            for (int ox0 = 0; ox0 < W; ox0 += tile_size) {

                int oy_max = oy0 + tile_size;
                if (oy_max > H)
                    oy_max = H;

                int ox_max = ox0 + tile_size;
                if (ox_max > W)
                    ox_max = W;

                for (int oy = oy0; oy < oy_max; ++oy) {
                    for (int ox = ox0; ox < ox_max; ++ox) {

                        const int base = oy * in_stride + ox;

                        float acc = 0.0f;

                        acc += in[base] * k00;
                        acc += in[base + 1] * k01;
                        acc += in[base + 2] * k02;

                        acc += in[base + in_stride] * k10;
                        acc += in[base + in_stride + 1] * k11;
                        acc += in[base + in_stride + 2] * k12;

                        acc += in[base + 2 * in_stride] * k20;
                        acc += in[base + 2 * in_stride + 1] * k21;
                        acc += in[base + 2 * in_stride + 2] * k22;

                        out[oy * W + ox] = acc;
                    }
                }
            }
        }

        return;
    }

    // =========================
    // K = 5 : Fully unrolled
    // =========================
    if (K == 5) {

        for (int oy0 = 0; oy0 < H; oy0 += tile_size) {
            for (int ox0 = 0; ox0 < W; ox0 += tile_size) {

                int oy_max = oy0 + tile_size;
                if (oy_max > H)
                    oy_max = H;

                int ox_max = ox0 + tile_size;
                if (ox_max > W)
                    ox_max = W;

                for (int oy = oy0; oy < oy_max; ++oy) {
                    for (int ox = ox0; ox < ox_max; ++ox) {

                        const int base = oy * in_stride + ox;

                        float acc = 0.0f;

                        // Row 0
                        acc += in[base] * ker[0];
                        acc += in[base + 1] * ker[1];
                        acc += in[base + 2] * ker[2];
                        acc += in[base + 3] * ker[3];
                        acc += in[base + 4] * ker[4];

                        // Row 1
                        acc += in[base + in_stride] * ker[5];
                        acc += in[base + in_stride + 1] * ker[6];
                        acc += in[base + in_stride + 2] * ker[7];
                        acc += in[base + in_stride + 3] * ker[8];
                        acc += in[base + in_stride + 4] * ker[9];

                        // Row 2
                        acc += in[base + 2 * in_stride] * ker[10];
                        acc += in[base + 2 * in_stride + 1] * ker[11];
                        acc += in[base + 2 * in_stride + 2] * ker[12];
                        acc += in[base + 2 * in_stride + 3] * ker[13];
                        acc += in[base + 2 * in_stride + 4] * ker[14];

                        // Row 3
                        acc += in[base + 3 * in_stride] * ker[15];
                        acc += in[base + 3 * in_stride + 1] * ker[16];
                        acc += in[base + 3 * in_stride + 2] * ker[17];
                        acc += in[base + 3 * in_stride + 3] * ker[18];
                        acc += in[base + 3 * in_stride + 4] * ker[19];

                        // Row 4
                        acc += in[base + 4 * in_stride] * ker[20];
                        acc += in[base + 4 * in_stride + 1] * ker[21];
                        acc += in[base + 4 * in_stride + 2] * ker[22];
                        acc += in[base + 4 * in_stride + 3] * ker[23];
                        acc += in[base + 4 * in_stride + 4] * ker[24];

                        out[oy * W + ox] = acc;
                    }
                }
            }
        }

        return;
    }

    // =========================
    // Generic fallback
    // =========================
    for (int oy0 = 0; oy0 < H; oy0 += tile_size) {
        for (int ox0 = 0; ox0 < W; ox0 += tile_size) {

            int oy_max = oy0 + tile_size;
            if (oy_max > H)
                oy_max = H;

            int ox_max = ox0 + tile_size;
            if (ox_max > W)
                ox_max = W;

            for (int oy = oy0; oy < oy_max; ++oy) {
                for (int ox = ox0; ox < ox_max; ++ox) {
                    float acc = 0.0f;
                    for (int ky = 0; ky < K; ++ky) {
                        for (int kx = 0; kx < K; ++kx) {
                            acc +=
                                in[(oy + ky) * in_stride + ox + kx] * ker[ky * K + kx];
                        }
                    }

                    out[oy * W + ox] = acc;
                }
            }
        }
    }
}

//--------------------------------simd+tiling---------------------------------------------

void implementation3(const float *in, float *out, const float *ker,
                     int H, int W, int K) {
    const int p = K / 2;
    const int in_stride = W + 2 * p; // padded row stride
    const int tile_size = 64;        // set cache tile size to 64

    // Outer loops: Step through the image in tile_size x tile_size blocks
    for (int oy = 0; oy < H; oy += tile_size) {
        for (int ox = 0; ox < W; ox += tile_size) {

            // Calculate boundaries to prevent out-of-bounds reading on edges
            int max_y = (oy + tile_size < H) ? tile_size : (H - oy);
            int max_x = (ox + tile_size < W) ? tile_size : (W - ox);

            // Inner loops: Process the current tile
            for (int ty = 0; ty < max_y; ++ty) {
                for (int tx = 0; tx < max_x; tx += 8) {
                    __m256 acc = _mm256_setzero_ps();

                    for (int ky = 0; ky < K; ++ky) {
                        const float *row = in + (oy + ty + ky) * in_stride + (ox + tx);
                        for (int kx = 0; kx < K; ++kx) {
                            // Load 8 consecutive input values
                            __m256 input = _mm256_loadu_ps(row + kx);

                            // Broadcast the single kernel value to all 8 lanes
                            __m256 weight = _mm256_set1_ps(ker[ky * K + kx]);

                            // acc = acc + (input * weight)
                            acc = _mm256_add_ps(acc, _mm256_mul_ps(input, weight));
                        }
                    }
                    // Store the 8 computed output pixels
                    _mm256_storeu_ps(&out[(oy + ty) * W + (ox + tx)], acc);
                }
            }
        }
    }
}

//--------------------------------reorder+tiling---------------------------------------------

void implementation4(const float *in, float *out, const float *ker,
                     int H, int W, int K) {
    const int p = K / 2;
    const int in_stride = W + 2 * p; // Padded row stride
    const int tile_size = 64;        // Cache tile size

    // Outer loops: Iterate over image tiles
    for (int oy = 0; oy < H; oy += tile_size) {
        for (int ox = 0; ox < W; ox += tile_size) {

            // Tile boundaries to handle image edges safely
            const int max_y = (oy + tile_size < H) ? tile_size : (H - oy);
            const int max_x = (ox + tile_size < W) ? tile_size : (W - ox);

            // Initialize and accumulate output while processing kernel values.
            for (int ky = 0; ky < K; ++ky) {
                for (int kx = 0; kx < K; ++kx) {
                    // Hoist weight load out of spatial loops into a single register
                    const float w = ker[ky * K + kx];

                    // Inner loops: Stream sequentially through the tile pixels
                    for (int ty = 0; ty < max_y; ++ty) {
                        const int in_row_offset = (oy + ty + ky) * in_stride + (ox + kx);
                        const int out_row_offset = (oy + ty) * W + ox;

                        for (int tx = 0; tx < max_x; ++tx) {
                            const float contribution = w * in[in_row_offset + tx];
                            if (ky == 0 && kx == 0) {
                                out[out_row_offset + tx] = contribution;
                            } else {
                                out[out_row_offset + tx] += contribution;
                            }
                        }
                    }
                }
            }
        }
    }
}

//--------------------------------simd+unrolling---------------------------------------------

void implementation5(const float* in, float* out, const float* ker,
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
