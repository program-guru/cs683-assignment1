// conv_tile.cpp  STAGE 3: CACHE TILING

#include "convolution.h"

void conv_tile(const float* in, float* out, const float* ker,
               int H, int W, int K) {
    // TODO(student): replace this placeholder with your tiled/blocked implementation.
    const int p = K / 2;
    const int in_stride = W + 2 * p;

    const int TILE = 16;
    // Increments by the size of the tile in output
    for (int oy0 = 0; oy0 < H; oy0 += TILE) {
        for (int ox0 = 0; ox0 < W; ox0 += TILE) {
            int oy_max = oy0 + TILE;
            if (oy_max > H)
                oy_max = H;
            int ox_max = ox0 + TILE;
            if (ox_max > W)
                ox_max = W;
            // Same as Naive convolution
            for (int oy = oy0; oy < oy_max; ++oy) {
                for (int ox = ox0; ox < ox_max; ++ox) {
                    float acc = 0.0f;
                    for (int ky = 0; ky < K; ++ky) {
                        for (int kx = 0; kx < K; ++kx) {
                            acc +=in[(oy + ky) * in_stride + (ox + kx)]*ker[ky * K + kx];
                        }
                    }
                    out[oy * W + ox] = acc;
                }
            }
        }
    }
}
