from pathlib import Path
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
from visualizations.color_scale import comparison_colors


def plot_simd(naive_df, simd_df):

    # ========================================================
    # Sort
    # ========================================================

    naive_df = naive_df.sort_values(
        'matrix_size'
    )

    simd_df = simd_df.sort_values(
        ['matrix_size', 'simd_register_width']
    )


    # ========================================================
    # Split SIMD configurations
    # ========================================================

    simd128 = simd_df[
        simd_df['simd_register_width'] == 128
    ].copy()

    simd256 = simd_df[
        simd_df['simd_register_width'] == 256
    ].copy()


    simd128 = simd128.sort_values(
        'matrix_size'
    )

    simd256 = simd256.sort_values(
        'matrix_size'
    )


    # ========================================================
    # Comparison DataFrame
    # ========================================================

    comparison_df = pd.DataFrame()

    comparison_df['matrix_size'] = (
        naive_df['matrix_size'].values
    )

    comparison_df['naive_instructions'] = (
        naive_df['instructions (combined)'].values
    )

    comparison_df['simd128_instructions'] = (
        simd128['instructions (actual)'].values
    )

    comparison_df['simd256_instructions'] = (
        simd256['instructions (actual)'].values
    )

    comparison_df['naive_misses'] = (
        naive_df['l1d_misses (combined)'].values
    )

    comparison_df['simd128_misses'] = (
        simd128['l1d_misses (actual)'].values
    )

    comparison_df['simd256_misses'] = (
        simd256['l1d_misses (actual)'].values
    )

    comparison_df['naive_mpki'] = (
        naive_df['mpki'].values
    )

    comparison_df['simd128_mpki'] = (
        simd128['mpki'].values
    )

    comparison_df['simd256_mpki'] = (
        simd256['mpki'].values
    )

    comparison_df['naive_miss_rate'] = (
        naive_df['miss_rate'].values
    )

    comparison_df['simd128_miss_rate'] = (
        simd128['miss_rate'].values
    )

    comparison_df['simd256_miss_rate'] = (
        simd256['miss_rate'].values
    )

    comparison_df['naive_loads'] = (
        naive_df['l1d_loads (combined)'].values
    )

    comparison_df['simd128_loads'] = (
        simd128['l1d_loads (actual)'].values
    )

    comparison_df['simd256_loads'] = (
        simd256['l1d_loads (actual)'].values
    )

    comparison_df['simd128_speedup'] = (
        simd128['speedup'].values
    )

    comparison_df['simd256_speedup'] = (
        simd256['speedup'].values
    )


    # ========================================================
    # Instruction Reduction
    # ========================================================

    comparison_df['simd128_instruction_reduction'] = (
        (
            comparison_df['naive_instructions']
            - comparison_df['simd128_instructions']
        )
        / comparison_df['naive_instructions']
    ) * 100

    comparison_df['simd256_instruction_reduction'] = (
        (
            comparison_df['naive_instructions']
            - comparison_df['simd256_instructions']
        )
        / comparison_df['naive_instructions']
    ) * 100


    # ========================================================
    # Cache Miss Change
    # ========================================================

    comparison_df['simd128_miss_change'] = (
        (
            comparison_df['simd128_misses']
            - comparison_df['naive_misses']
        )
        / comparison_df['naive_misses']
    ) * 100

    comparison_df['simd256_miss_change'] = (
        (
            comparison_df['simd256_misses']
            - comparison_df['naive_misses']
        )
        / comparison_df['naive_misses']
    ) * 100


    # ========================================================
    # Plot
    # ========================================================

    sns.set_theme(style="whitegrid")

    fig, axes = plt.subplots(
        2, 3,
        figsize=(21, 12)
    )


    # ========================================================
    # 1. Speedup
    # ========================================================

    axes[0, 0].plot(
        comparison_df['matrix_size'],
        comparison_df['simd128_speedup'],
        marker='o',
        linewidth=2.5,
        color=comparison_colors(
            comparison_df['simd128_speedup'],
            center=1.0
        )[0],
        label='SIMD 128-bit'
    )

    axes[0, 0].plot(
        comparison_df['matrix_size'],
        comparison_df['simd256_speedup'],
        marker='s',
        linewidth=2.5,
        color=comparison_colors(
            comparison_df['simd256_speedup'],
            center=1.0
        )[0],
        label='SIMD 256-bit'
    )

    axes[0, 0].axhline(
        y=1.0,
        linestyle='--',
        linewidth=1.5,
        label='Naive Baseline'
    )

    axes[0, 0].set_title(
        'SIMD Speedup vs Matrix Size',
        fontweight='bold'
    )

    axes[0, 0].set_xlabel('Matrix Size')
    axes[0, 0].set_ylabel('Speedup')

    axes[0, 0].set_xticks(
        comparison_df['matrix_size']
    )

    axes[0, 0].legend()


    # ========================================================
    # 2. Instructions
    # ========================================================

    x = range(len(comparison_df))
    width = 0.25

    axes[0, 1].bar(
        [i - width for i in x],
        comparison_df['naive_instructions'],
        width,
        label='Naive'
    )

    axes[0, 1].bar(
        list(x),
        comparison_df['simd128_instructions'],
        width,
        label='SIMD 128-bit'
    )

    axes[0, 1].bar(
        [i + width for i in x],
        comparison_df['simd256_instructions'],
        width,
        label='SIMD 256-bit'
    )

    axes[0, 1].set_title(
        'Instruction Count Comparison',
        fontweight='bold'
    )

    axes[0, 1].set_xlabel('Matrix Size')
    axes[0, 1].set_ylabel('Instructions')

    axes[0, 1].set_xticks(list(x))

    axes[0, 1].set_xticklabels(
        comparison_df['matrix_size']
    )

    axes[0, 1].ticklabel_format(
        style='sci',
        axis='y',
        scilimits=(0, 0)
    )

    axes[0, 1].legend()


    # ========================================================
    # 3. MPKI
    # ========================================================

    axes[0, 2].plot(
        comparison_df['matrix_size'],
        comparison_df['naive_mpki'],
        marker='o',
        linewidth=2.5,
        label='Naive'
    )

    axes[0, 2].plot(
        comparison_df['matrix_size'],
        comparison_df['simd128_mpki'],
        marker='s',
        linewidth=2.5,
        label='SIMD 128-bit'
    )

    axes[0, 2].plot(
        comparison_df['matrix_size'],
        comparison_df['simd256_mpki'],
        marker='^',
        linewidth=2.5,
        label='SIMD 256-bit'
    )

    axes[0, 2].set_title(
        'L1-D MPKI Comparison',
        fontweight='bold'
    )

    axes[0, 2].set_xlabel('Matrix Size')
    axes[0, 2].set_ylabel('L1-D MPKI')

    axes[0, 2].set_xticks(
        comparison_df['matrix_size']
    )

    axes[0, 2].legend()


    # ========================================================
    # 4. Instruction Reduction
    # ========================================================

    axes[1, 0].plot(
        comparison_df['matrix_size'],
        comparison_df['simd128_instruction_reduction'],
        marker='o',
        linewidth=2.5,
        color=comparison_colors(
            comparison_df['simd128_instruction_reduction']
        )[0],
        label='SIMD 128-bit'
    )

    axes[1, 0].plot(
        comparison_df['matrix_size'],
        comparison_df['simd256_instruction_reduction'],
        marker='s',
        linewidth=2.5,
        color=comparison_colors(
            comparison_df['simd256_instruction_reduction']
        )[0],
        label='SIMD 256-bit'
    )

    axes[1, 0].axhline(
        y=0,
        linestyle='--',
        linewidth=1.5
    )

    axes[1, 0].set_title(
        'Instruction Reduction vs Naive',
        fontweight='bold'
    )

    axes[1, 0].set_xlabel('Matrix Size')
    axes[1, 0].set_ylabel(
        'Instruction Reduction (%)'
    )

    axes[1, 0].set_xticks(
        comparison_df['matrix_size']
    )

    axes[1, 0].legend()


    # ========================================================
    # 5. L1-D Miss Rate vs Matrix Size
    # ========================================================

    axes[1, 1].plot(
        comparison_df['matrix_size'],
        comparison_df['naive_miss_rate'] * 100,
        marker='o',
        linewidth=2.5,
        label='Naive'
    )

    axes[1, 1].plot(
        comparison_df['matrix_size'],
        comparison_df['simd128_miss_rate'] * 100,
        marker='s',
        linewidth=2.5,
        label='SIMD 128-bit'
    )

    axes[1, 1].plot(
        comparison_df['matrix_size'],
        comparison_df['simd256_miss_rate'] * 100,
        marker='^',
        linewidth=2.5,
        label='SIMD 256-bit'
    )

    axes[1, 1].set_title(
        'L1-D Miss Rate vs Matrix Size',
        fontweight='bold'
    )

    axes[1, 1].set_xlabel('Matrix Size')

    axes[1, 1].set_ylabel(
        'L1-D Miss Rate (%)'
    )

    axes[1, 1].set_xticks(
        comparison_df['matrix_size']
    )

    axes[1, 1].legend()

    # ========================================================
    # 6. Heatmap
    # ========================================================

    heatmap_data = comparison_df[
        [
            'simd128_speedup',
            'simd256_speedup'
        ]
    ].T

    heatmap_data.columns = (
        comparison_df['matrix_size']
    )

    heatmap_data.index = [
        'SIMD 128-bit',
        'SIMD 256-bit'
    ]

    sns.heatmap(
        heatmap_data,
        annot=True,
        fmt='.2f',
        cmap='RdYlGn',
        center=1.0,
        linewidths=0.5,
        ax=axes[1, 2],
        cbar_kws={'label': 'Speedup'}
    )

    axes[1, 2].set_title(
        'SIMD Speedup Heatmap',
        fontweight='bold'
    )

    axes[1, 2].set_xlabel('Matrix Size')
    axes[1, 2].set_ylabel(
        'SIMD Configuration'
    )


    # ========================================================
    # Formatting
    # ========================================================

    for ax in axes.flat:
        ax.tick_params(
            axis='x',
            rotation=45
        )


    fig.suptitle(
        'Performance Analysis of SIMD Optimization',
        fontsize=20,
        fontweight='bold'
    )

    plt.tight_layout(
        w_pad=3.0,
        h_pad=3.0,
        rect=(0, 0, 1, 0.96)
    )
    
    output_path = Path(__file__).resolve().parents[2] / 'results' / 'simd_analysis.png'
    fig.savefig(output_path, dpi=300, bbox_inches='tight')