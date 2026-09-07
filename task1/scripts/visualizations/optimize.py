import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
from pathlib import Path
from matplotlib.container import BarContainer


# ============================================================
# Configuration
# ============================================================

IMPLEMENTATION_NAMES = {
    1: 'Reorder + Unroll',
    2: 'Tiling + Unroll',
    3: 'Tiling + SIMD',
    4: 'Reorder + Tiling',
    5: 'SIMD + Unrolling'
}


# ============================================================
# Main Visualization
# ============================================================

def plot_optimize(optimized_df):

    # ============================================================
    # Create copy
    # ============================================================

    df = optimized_df.copy()


    # ============================================================
    # Add implementation names
    # ============================================================

    df['implementation_name'] = (
        df['implementation']
        .map(IMPLEMENTATION_NAMES)
    )


    # ============================================================
    # Sort data
    # ============================================================

    df = df.sort_values(
        by=[
            'implementation',
            'kernel_size',
            'matrix_size'
        ]
    )


    # ============================================================
    # 1. Average Speedup by Matrix Size
    #
    # Average across all kernel sizes
    # ============================================================

    matrix_speedup_df = (
        df
        .groupby(
            [
                'matrix_size',
                'implementation',
                'implementation_name'
            ],
            as_index=False
        )
        .agg(
            avg_speedup=(
                'speedup',
                'mean'
            )
        )
    )


    # ============================================================
    # 2. Average Speedup by Kernel Size
    #
    # Average across all matrix sizes
    # ============================================================

    kernel_speedup_df = (
        df
        .groupby(
            [
                'kernel_size',
                'implementation',
                'implementation_name'
            ],
            as_index=False
        )
        .agg(
            avg_speedup=(
                'speedup',
                'mean'
            )
        )
    )


    # ============================================================
    # 3. Average Instruction Count and MPKI
    #
    # Average across all kernel sizes
    # Grouped by matrix size and implementation
    # ============================================================

    metrics_df = (
        df
        .groupby(
            [
                'matrix_size',
                'implementation',
                'implementation_name'
            ],
            as_index=False
        )
        .agg(
            avg_instructions=(
                'instructions (combined)',
                'mean'
            ),

            avg_mpki=(
                'mpki',
                'mean'
            )
        )
    )


    # ============================================================
    # Sort all DataFrames
    # ============================================================

    matrix_speedup_df = (
        matrix_speedup_df
        .sort_values(
            [
                'matrix_size',
                'implementation'
            ]
        )
    )


    kernel_speedup_df = (
        kernel_speedup_df
        .sort_values(
            [
                'kernel_size',
                'implementation'
            ]
        )
    )


    metrics_df = (
        metrics_df
        .sort_values(
            [
                'matrix_size',
                'implementation'
            ]
        )
    )


    # ============================================================
    # Display Results
    # ============================================================

    print("\n========================================")
    print("OPTIMIZATION ANALYSIS")
    print("========================================")


    print("\n----------------------------------------")
    print("AVERAGE SPEEDUP BY MATRIX SIZE")
    print("(Average Across All Kernel Sizes)")
    print("----------------------------------------")

    print(matrix_speedup_df)


    print("\n----------------------------------------")
    print("AVERAGE SPEEDUP BY KERNEL SIZE")
    print("(Average Across All Matrix Sizes)")
    print("----------------------------------------")

    print(kernel_speedup_df)


    print("\n----------------------------------------")
    print("AVERAGE INSTRUCTIONS AND MPKI")
    print("(Average Across All Kernel Sizes)")
    print("----------------------------------------")

    print(metrics_df)


    # ============================================================
    # Plotting Style
    # ============================================================

    sns.set_theme(
        style="whitegrid"
    )


    # ============================================================
    # Output Directory
    # ============================================================

    output_dir = (
        Path(__file__)
        .resolve()
        .parents[2]
        / 'results'
    )


    # ============================================================
    # PLOT 1
    #
    # SPEEDUP VS MATRIX SIZE
    # ============================================================

    fig1, ax1 = plt.subplots(
        figsize=(18, 9)
    )


    sns.barplot(
        data=matrix_speedup_df,
        x='matrix_size',
        y='avg_speedup',
        hue='implementation_name',
        ax=ax1
    )


    # ------------------------------------------------------------
    # Baseline
    # ------------------------------------------------------------

    ax1.axhline(
        y=1.0,
        linestyle='--',
        linewidth=2,
        color='black',
        label='Baseline'
    )


    # ------------------------------------------------------------
    # Labels
    # ------------------------------------------------------------

    ax1.set_title(
        'Average Speedup vs Matrix Size',
        fontsize=18,
        fontweight='bold'
    )

    ax1.set_xlabel(
        'Matrix Size',
        fontweight='bold'
    )

    ax1.set_ylabel(
        'Average Speedup',
        fontweight='bold'
    )


    # ------------------------------------------------------------
    # Bar Labels
    # ------------------------------------------------------------

    for container in ax1.containers:

      if isinstance(container, BarContainer):

         ax1.bar_label(
               container,
               fmt='%.2fx',
               padding=2,
               fontsize=7
         )


    # ------------------------------------------------------------
    # Grid
    # ------------------------------------------------------------

    ax1.grid(
        axis='y',
        alpha=0.3
    )


    # ------------------------------------------------------------
    # Legend
    # ------------------------------------------------------------

    ax1.legend(
        title='Implementation'
    )


    # ------------------------------------------------------------
    # Layout
    # ------------------------------------------------------------

    plt.tight_layout()


    # ------------------------------------------------------------
    # Save
    # ------------------------------------------------------------

    output_path_1 = (
        output_dir
        / 'task1_speedup_vs_size.png'
    )

    fig1.savefig(
        output_path_1,
        dpi=300,
        bbox_inches='tight'
    )

    plt.close(fig1)


    # ============================================================
    # PLOT 2
    #
    # SPEEDUP VS KERNEL SIZE
    # ============================================================

    fig2, ax2 = plt.subplots(
        figsize=(14, 9)
    )


    sns.barplot(
        data=kernel_speedup_df,
        x='kernel_size',
        y='avg_speedup',
        hue='implementation_name',
        ax=ax2
    )


    # ------------------------------------------------------------
    # Baseline
    # ------------------------------------------------------------

    ax2.axhline(
        y=1.0,
        linestyle='--',
        linewidth=2,
        color='black',
        label='Baseline'
    )


    # ------------------------------------------------------------
    # Labels
    # ------------------------------------------------------------

    ax2.set_title(
        'Average Speedup vs Kernel Size',
        fontsize=18,
        fontweight='bold'
    )

    ax2.set_xlabel(
        'Kernel Size',
        fontweight='bold'
    )

    ax2.set_ylabel(
        'Average Speedup',
        fontweight='bold'
    )


    # ------------------------------------------------------------
    # Bar Labels
    # ------------------------------------------------------------

    for container in ax2.containers:

        if isinstance(container, BarContainer):

            ax2.bar_label(
                container,
                fmt='%.2fx',
                padding=2,
                fontsize=8
            )


    # ------------------------------------------------------------
    # Grid
    # ------------------------------------------------------------

    ax2.grid(
        axis='y',
        alpha=0.3
    )


    # ------------------------------------------------------------
    # Legend
    # ------------------------------------------------------------

    ax2.legend(
        title='Implementation'
    )


    # ------------------------------------------------------------
    # Layout
    # ------------------------------------------------------------

    plt.tight_layout()


    # ------------------------------------------------------------
    # Save
    # ------------------------------------------------------------

    output_path_2 = (
        output_dir
        / 'task1_speedup_vs_kernel.png'
    )

    fig2.savefig(
        output_path_2,
        dpi=300,
        bbox_inches='tight'
    )

    plt.close(fig2)


    # ============================================================
    # PLOT 3
    #
    # INSTRUCTION COUNT + MPKI
    # ============================================================

    fig3, axes = plt.subplots(
        1,
        2,
        figsize=(20, 8)
    )


    # ============================================================
    # 3A. INSTRUCTION COUNT VS MATRIX SIZE
    # ============================================================

    sns.lineplot(
        data=metrics_df,
        x='matrix_size',
        y='avg_instructions',
        hue='implementation_name',
        marker='o',
        linewidth=2.5,
        ax=axes[0]
    )


    axes[0].set_title(
        'Average Instruction Count vs Matrix Size',
        fontsize=16,
        fontweight='bold'
    )

    axes[0].set_xlabel(
        'Matrix Size',
        fontweight='bold'
    )

    axes[0].set_ylabel(
        'Average Instructions',
        fontweight='bold'
    )


    axes[0].ticklabel_format(
        style='sci',
        axis='y',
        scilimits=(0, 0)
    )


    axes[0].set_xticks(
        sorted(
            metrics_df['matrix_size'].unique()
        )
    )


    axes[0].tick_params(
        axis='x',
        rotation=45
    )


    axes[0].grid(
        alpha=0.3
    )


    axes[0].legend(
        title='Implementation'
    )


    # ============================================================
    # 3B. MPKI VS MATRIX SIZE
    # ============================================================

    sns.lineplot(
        data=metrics_df,
        x='matrix_size',
        y='avg_mpki',
        hue='implementation_name',
        marker='o',
        linewidth=2.5,
        ax=axes[1]
    )


    axes[1].set_title(
        'Average L1-D MPKI vs Matrix Size',
        fontsize=16,
        fontweight='bold'
    )

    axes[1].set_xlabel(
        'Matrix Size',
        fontweight='bold'
    )

    axes[1].set_ylabel(
        'Average L1-D MPKI',
        fontweight='bold'
    )


    axes[1].set_xticks(
        sorted(
            metrics_df['matrix_size'].unique()
        )
    )


    axes[1].tick_params(
        axis='x',
        rotation=45
    )


    axes[1].grid(
        alpha=0.3
    )


    axes[1].legend(
        title='Implementation'
    )


    # ============================================================
    # Overall Title
    # ============================================================

    fig3.suptitle(
        'Instruction Count and Cache Efficiency Analysis',
        fontsize=18,
        fontweight='bold'
    )


    # ============================================================
    # Layout
    # ============================================================

    plt.tight_layout(
        w_pad=4.0,
        rect=(0, 0, 1, 0.93)
    )


    # ============================================================
    # Save
    # ============================================================

    output_path_3 = (
        output_dir
        / 'optimize.png'
    )

    fig3.savefig(
        output_path_3,
        dpi=300,
        bbox_inches='tight'
    )

    plt.close(fig3)


    # ============================================================
    # Output
    # ============================================================

    print("\n========================================")
    print("VISUALIZATIONS GENERATED")
    print("========================================")

    print(
        f"\n1. {output_path_1}"
    )

    print(
        f"2. {output_path_2}"
    )

    print(
        f"3. {output_path_3}"
    )