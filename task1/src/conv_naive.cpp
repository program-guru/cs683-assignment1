// conv_naive.cpp  PROVIDED reference implementation.
//
// This is the correctness baseline AND the performance baseline that all speedups
// are measured against. Do NOT modify this file.

#include "convolution.h"

void conv_naive(const float* in, float* out, const float* ker,
                int H, int W, int K) {
    // Padding is used so that we don't have to worry about boundaries for the kernal
    // Padding is based on Kernal size and not the input/output size...
    // Calculates the padding
    const int p = K / 2; 

    // The input is a 1d array, so to get the row size we do this, 
    // tells us how many elements we need to skip to move from one row to the next.
    const int in_stride = W + 2 * p;  // padded row stride

    // Goes thru each output element(oy,ox)
    for (int oy = 0; oy < H; ++oy) {
        for (int ox = 0; ox < W; ++ox) {
            float acc = 0.0f;
            // Goes thru each kernel element(ky,kx)
            for (int ky = 0; ky < K; ++ky) {
                for (int kx = 0; kx < K; ++kx) {
                    //calculates the values sum of all (in[y][x]*ker[y][x])
                    acc += in[(oy + ky) * in_stride + (ox + kx)] * ker[ky * K + kx];
                }
            }
            //fills output
            out[oy * W + ox] = acc;
        }
    }
}
