# CS683: Programming Assignment 1 - Dhurandhar Programming

**Course:** CS683: Advanced Computer Architecture, Autumn 2026, IIT Bombay

## Overview

This repository bridges the gap between high-level algorithms and bare-metal execution by deploying hardware-aware optimizations for two foundational computational engines: 2D Convolution and Single-Precision Matrix Multiplication (SGEMM). Instead of letting naive code bottleneck on memory bandwidth, these implementations bend silicon to their will—transforming cache-thrashing loops into streamlined pipelines through 256-bit AVX2 vectorization, precise temporal/spatial data reuse, and aggressive memory-latency hiding.

All experiments, benchmarking, and hardware performance counter profiling (`perf`) require an Intel-based x86 architecture with AVX2 and FMA support.

---

## Repository Architecture

```text
.
├── task1/
│   └── src/
│       ├── conv_reorder.cpp           # Loop reordering for spatial locality
│       ├── conv_unroll.cpp            # Loop unrolling for kernel weights
│       ├── conv_simd.cpp              # 256-bit AVX2 vectorization
│       ├── conv_tile.cpp              # L1-D cache blocking/tiling
│       └── conv_optimized.cpp         # Combined SIMD + Unrolling kernel
├── task2/
│   └── src/
│       ├── matmul_prefetch.cpp        # Prefetching experiments
│       ├── matmul_simd.cpp            # AVX2 vs 128-bit SIMD comparisons
│       └── matmul_optimized.cpp       # Blocked + Tiled + Vectorized + Prefetched SGEMM
└── plots/                           
   ├── task1_speedup_vs_size.png      # Speedup scaling across varying image dimensions
   ├── task1_speedup_vs_kernel.png    # Performance impact across kernel sizes, highlighting peak 11.95x speedup
   ├── task2_speedup_vs_size.png      # SGEMM speedup scaling across matrix dimensions (24x–28x peak)
   ├── task2_prefetch_degree.png      # Memory latency hiding across lookahead distances (256–512 elements)
   └── task2_prefetch_level.png       # Target cache level analysis (L3/LLC vs L0/L1 cache pollution)

```
