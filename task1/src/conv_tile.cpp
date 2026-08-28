// conv_tile.cpp  STAGE 3: CACHE TILING

#include "convolution.h"

void conv_tile(const float* in, float* out, const float* ker,
               int H, int W, int K) {
    const int p = K / 2;
    const int in_stride = W + 2 * p;  // padded row stride
    const int tile_size = 32;  // tile size for cache tiling

    for (int oy = 0; oy < H; oy += tile_size) {
        for (int ox = 0; ox < W; ox += tile_size) {
            // Process a tile of size tile_size x tile_size
            for (int ty = 0; ty < tile_size && (oy + ty) < H; ++ty) {
                for (int tx = 0; tx < tile_size && (ox + tx) < W; ++tx) {
                    float acc = 0.0f;
                    for (int ky = 0; ky < K; ++ky) {
                        for (int kx = 0; kx < K; ++kx) {
                            acc += in[(oy + ty + ky) * in_stride + (ox + tx + kx)] * ker[ky * K + kx];
                        }
                    }
                    out[(oy + ty) * W + (ox + tx)] = acc;
                }
            }
        }
    }
}