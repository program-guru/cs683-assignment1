#include "convolution.h"

void conv_unroll(const float* in, float* out, const float* ker,
                 int H, int W, int K) {

    const int p = K / 2;
    const int in_stride = W + 2 * p;

    // Special optimization for K = 5
    if (K == 5) {
        // Store kernel values once
        const float k00 = ker[0];
        const float k01 = ker[1];
        const float k02 = ker[2];
        const float k03 = ker[3];
        const float k04 = ker[4];

        const float k10 = ker[5];
        const float k11 = ker[6];
        const float k12 = ker[7];
        const float k13 = ker[8];
        const float k14 = ker[9];

        const float k20 = ker[10];
        const float k21 = ker[11];
        const float k22 = ker[12];
        const float k23 = ker[13];
        const float k24 = ker[14];

        const float k30 = ker[15];
        const float k31 = ker[16];
        const float k32 = ker[17];
        const float k33 = ker[18];
        const float k34 = ker[19];

        const float k40 = ker[20];
        const float k41 = ker[21];
        const float k42 = ker[22];
        const float k43 = ker[23];
        const float k44 = ker[24];

        for (int oy = 0; oy < H; ++oy) {
            for (int ox = 0; ox < W; ++ox) {

                const int base = oy * in_stride + ox;

                float acc = 0.0f;

                acc += in[base]                   * k00;
                acc += in[base + 1]               * k01;
                acc += in[base + 2]               * k02;
                acc += in[base + 3]               * k03;
                acc += in[base + 4]               * k04;

                acc += in[base + in_stride]       * k10;
                acc += in[base + in_stride + 1]   * k11;
                acc += in[base + in_stride + 2]   * k12;
                acc += in[base + in_stride + 3]   * k13;
                acc += in[base + in_stride + 4]   * k14;

                acc += in[base + 2 * in_stride]     * k20;
                acc += in[base + 2 * in_stride + 1] * k21;
                acc += in[base + 2 * in_stride + 2] * k22;
                acc += in[base + 2 * in_stride + 3] * k23;
                acc += in[base + 2 * in_stride + 4] * k24;

                acc += in[base + 3 * in_stride]     * k30;
                acc += in[base + 3 * in_stride + 1] * k31;
                acc += in[base + 3 * in_stride + 2] * k32;
                acc += in[base + 3 * in_stride + 3] * k33;
                acc += in[base + 3 * in_stride + 4] * k34;

                acc += in[base + 4 * in_stride]     * k40;
                acc += in[base + 4 * in_stride + 1] * k41;
                acc += in[base + 4 * in_stride + 2] * k42;
                acc += in[base + 4 * in_stride + 3] * k43;
                acc += in[base + 4 * in_stride + 4] * k44;

                out[oy * W + ox] = acc;
            }
        }

        return;
    }

    // Special optimization for K = 3
    if (K == 3){
        const float k00 = ker[0];
        const float k01 = ker[1];
        const float k02 = ker[2];

        const float k10 = ker[3];
        const float k11 = ker[4];
        const float k12 = ker[5];

        const float k20 = ker[6];
        const float k21 = ker[7];
        const float k22 = ker[8];

        for (int oy = 0; oy < H; ++oy) {
            for (int ox = 0; ox < W; ++ox) {
                const int base = oy * in_stride + ox;

                float acc = 0.0f;

                acc += in[base]                   * k00;
                acc += in[base + 1]               * k01;
                acc += in[base + 2]               * k02;

                acc += in[base + in_stride]       * k10;
                acc += in[base + in_stride + 1]   * k11;
                acc += in[base + in_stride + 2]   * k12;

                acc += in[base + 2 * in_stride]     * k20;
                acc += in[base + 2 * in_stride + 1] * k21;
                acc += in[base + 2 * in_stride + 2] * k22;

                out[oy * W + ox] = acc;
            }
        }

        return;
    }

    for (int oy = 0; oy < H; ++oy) {
        for (int ox = 0; ox < W; ++ox) {
            float acc = 0.0f;
            for (int ky = 0; ky < K; ++ky) {
                const int kend = K - (K % 3);
                for (int kx = 0; kx < kend; kx += 3) {
                    acc += in[(oy + ky) * in_stride + (ox + kx)] * ker[ky * K + kx];
                    acc += in[(oy + ky) * in_stride + (ox + kx + 1)] * ker[ky * K + kx + 1];
                    acc += in[(oy + ky) * in_stride + (ox + kx + 2)] * ker[ky * K + kx + 2];
                }
                for (int kx = kend; kx < K; ++kx) {
                    acc += in[(oy + ky) * in_stride + (ox + kx)] * ker[ky * K + kx];
                }
            }
            out[oy * W + ox] = acc;
        }
    }
}