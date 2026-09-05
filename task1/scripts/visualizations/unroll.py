import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
from pathlib import Path

def plot_unroll(naive_df, unroll_df):

    # ============================================================
    # Sort by matrix size
    # ============================================================

    naive_df = naive_df.sort_values(
        'matrix_size'
    ).copy()

    unroll_df = unroll_df.sort_values(
        'matrix_size'
    ).copy()

    # ============================================================
    # Create comparison DataFrame
    # ============================================================

    comparison_df = pd.DataFrame()

    comparison_df['matrix_size'] = (
        naive_df['matrix_size'].values
    )

    comparison_df['naive_mpki'] = (
        naive_df['mpki'].values
    )

    comparison_df['unroll_mpki'] = (
        unroll_df['mpki'].values
    )

    comparison_df['naive_misses'] = (
        naive_df['l1d_misses (combined)'].values
    )

    comparison_df['unroll_misses'] = (
        unroll_df['l1d_misses (actual)'].values
    )

    comparison_df['naive_instructions'] = (
        naive_df['instructions (combined)'].values
    )

    comparison_df['unroll_instructions'] = (
        unroll_df['instructions (actual)'].values
    )

    comparison_df['speedup'] = (
        unroll_df['speedup'].values
    )

    # ============================================================
    # Calculate improvement metrics
    # ============================================================

    # MPKI reduction percentage
    comparison_df['mpki_reduction'] = (
        (
            comparison_df['naive_mpki']
            - comparison_df['unroll_mpki']
        )
        / comparison_df['naive_mpki']
    ) * 100

    # Cache miss reduction percentage
    comparison_df['miss_reduction'] = (
        (
            comparison_df['naive_misses']
            - comparison_df['unroll_misses']
        )
        / comparison_df['naive_misses']
    ) * 100

    # Instruction count change percentage
    comparison_df['instruction_change'] = (
        (
            comparison_df['unroll_instructions']
            - comparison_df['naive_instructions']
        )
        / comparison_df['naive_instructions']
    ) * 100

    # ============================================================
    # Display data
    # ============================================================

    print("\n========================================")
    print("LOOP UNROLLING ANALYSIS")
    print("========================================")

    print("\nLoop Unrolling Comparison:")
    print(comparison_df)

    # ============================================================
    # Plotting style
    # ============================================================

    sns.set_theme(style="whitegrid")

    fig, axes = plt.subplots(
        2,
        2,
        figsize=(16, 12)
    )

    # ============================================================
    # 1. Speedup Bar Plot
    # ============================================================

    sns.barplot(
        data=comparison_df,
        x='matrix_size',
        y='speedup',
        ax=axes[0, 0]
    )

    axes[0, 0].axhline(
        y=1.0,
        linestyle='--',
        linewidth=2,
        label='Naive Baseline'
    )

    axes[0, 0].set_title(
        'Loop Unrolling: Speedup',
        fontweight='bold'
    )

    axes[0, 0].set_xlabel('Matrix Size')
    axes[0, 0].set_ylabel('Speedup')

    axes[0, 0].legend()

    # Add values above bars
    for container in axes[0, 0].containers:
        axes[0, 0].bar_label(
            container,
            fmt='%.2fx',
            padding=3
        )

    # ============================================================
    # 2. MPKI Comparison
    # ============================================================

    x = range(len(comparison_df))
    width = 0.35

    axes[0, 1].bar(
        [i - width / 2 for i in x],
        comparison_df['naive_mpki'],
        width,
        label='Naive'
    )

    axes[0, 1].bar(
        [i + width / 2 for i in x],
        comparison_df['unroll_mpki'],
        width,
        label='Unrolled'
    )

    axes[0, 1].set_title(
        'L1-D Cache MPKI Comparison',
        fontweight='bold'
    )

    axes[0, 1].set_xlabel('Matrix Size')
    axes[0, 1].set_ylabel('L1-D MPKI')

    axes[0, 1].set_xticks(
        list(x)
    )

    axes[0, 1].set_xticklabels(
        comparison_df['matrix_size']
    )

    axes[0, 1].legend()

    # ============================================================
    # 3. MPKI Reduction
    # ============================================================

    sns.barplot(
        data=comparison_df,
        x='matrix_size',
        y='mpki_reduction',
        ax=axes[1, 0]
    )

    axes[1, 0].axhline(
        y=0,
        linestyle='--',
        linewidth=1.5
    )

    axes[1, 0].set_title(
        'L1-D MPKI Reduction from Loop Unrolling',
        fontweight='bold'
    )

    axes[1, 0].set_xlabel('Matrix Size')
    axes[1, 0].set_ylabel('MPKI Reduction (%)')

    # Add percentages
    for container in axes[1, 0].containers:
        axes[1, 0].bar_label(
            container,
            fmt='%.1f%%',
            padding=3
        )

    # ============================================================
    # 4. Instruction Count Comparison
    # ============================================================

    axes[1, 1].plot(
        comparison_df['matrix_size'],
        comparison_df['naive_instructions'],
        marker='o',
        linewidth=2.5,
        label='Naive'
    )

    axes[1, 1].plot(
        comparison_df['matrix_size'],
        comparison_df['unroll_instructions'],
        marker='s',
        linewidth=2.5,
        label='Unrolled'
    )

    axes[1, 1].set_title(
        'Instruction Count: Naive vs Unrolled',
        fontweight='bold'
    )

    axes[1, 1].set_xlabel('Matrix Size')
    axes[1, 1].set_ylabel('Instructions')

    axes[1, 1].ticklabel_format(
        style='sci',
        axis='y',
        scilimits=(0, 0)
    )

    axes[1, 1].legend()

    # ============================================================
    # Overall title
    # ============================================================

    fig.suptitle(
        'Performance Analysis of Loop Unrolling',
        fontsize=18,
        fontweight='bold'
    )

    # ============================================================
    # Layout
    # ============================================================

    plt.tight_layout(
        w_pad=3.0,
        h_pad=3.0,
        rect=(0, 0, 1, 0.96)
    )
    
    output_path = Path(__file__).resolve().parents[2] / 'results' / 'unroll_analysis.png'
    fig.savefig(output_path, dpi=300, bbox_inches='tight')