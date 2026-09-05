from pathlib import Path
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns


def plot_naive(naive_df):

    # ============================================================
    # Sort by matrix size
    # ============================================================

    naive_df = naive_df.sort_values(
        'matrix_size'
    ).copy()

    # ============================================================
    # Display data
    # ============================================================

    print("\n========================================")
    print("NAIVE ANALYSIS")
    print("========================================")

    print("\nNaive Results:")
    print(naive_df)

    # ============================================================
    # Plotting style
    # ============================================================

    sns.set_theme(style="whitegrid")

    fig, axes = plt.subplots(
        1,
        3,
        figsize=(18, 6)
    )

    # Get matrix sizes directly from dataset
    matrix_sizes = sorted(
        naive_df['matrix_size'].unique()
    )

    # ============================================================
    # 1. Instructions vs Matrix Size
    # ============================================================

    sns.lineplot(
        data=naive_df,
        x='matrix_size',
        y='instructions (combined)',
        marker='o',
        linewidth=2.5,
        ax=axes[0]
    )

    axes[0].set_title(
        'Naive: Instructions vs Matrix Size',
        fontweight='bold'
    )

    axes[0].set_xlabel('Matrix Size')
    axes[0].set_ylabel('Instructions')

    axes[0].set_xticks(
        matrix_sizes
    )

    # ============================================================
    # 2. L1-D Misses vs Matrix Size
    # ============================================================

    sns.lineplot(
        data=naive_df,
        x='matrix_size',
        y='l1d_misses (combined)',
        marker='o',
        linewidth=2.5,
        ax=axes[1]
    )

    axes[1].set_title(
        'Naive: L1-D Misses vs Matrix Size',
        fontweight='bold'
    )

    axes[1].set_xlabel('Matrix Size')
    axes[1].set_ylabel('L1-D Misses')

    axes[1].set_xticks(
        matrix_sizes
    )

    # ============================================================
    # 3. MPKI vs Matrix Size
    # ============================================================

    sns.lineplot(
        data=naive_df,
        x='matrix_size',
        y='mpki',
        marker='o',
        linewidth=2.5,
        ax=axes[2]
    )

    axes[2].set_title(
        'Naive: MPKI vs Matrix Size',
        fontweight='bold'
    )

    axes[2].set_xlabel('Matrix Size')
    axes[2].set_ylabel('MPKI')

    # ============================================================
    # Overall Title
    # ============================================================

    fig.suptitle(
        'Performance Analysis of Naive Convolution',
        fontsize=18,
        fontweight='bold'
    )

    # ============================================================
    # Final Layout
    # ============================================================

    plt.tight_layout(
        w_pad=3.0,
        h_pad=3.0,
        rect=(0, 0, 1, 0.96)
    )

    output_path = Path(__file__).resolve().parents[2] / 'results' / 'naive_analysis.png'
    fig.savefig(output_path, dpi=300, bbox_inches='tight')