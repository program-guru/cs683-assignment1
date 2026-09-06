import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
from pathlib import Path
from visualizations.color_scale import comparison_colors

def plot_tile(naive_df, tile_df):

    # =========================================================
    # Sort data
    # =========================================================

    naive_df = naive_df.sort_values(
        'matrix_size'
    ).copy()

    tile_df = tile_df.sort_values(
        ['matrix_size', 'tile_size']
    ).copy()

    # =========================================================
    # Calculate cache miss increase relative to Naive
    # =========================================================

    naive_miss_map = (
        naive_df
        .set_index('matrix_size')['l1d_misses (combined)']
    )

    tile_df['naive_misses'] = (
        tile_df['matrix_size'].map(naive_miss_map)
    )

    tile_df['miss_increase'] = (
        (
            tile_df['l1d_misses (actual)']
            - tile_df['naive_misses']
        )
        / tile_df['naive_misses']
    ) * 100

    # =========================================================
    # Average results by tile size
    # =========================================================

    avg_tile = (
        tile_df
        .groupby('tile_size')
        .agg(
            avg_speedup=('speedup', 'mean'),
            avg_mpki=('mpki', 'mean'),
            avg_miss_rate=('miss_rate', 'mean'),
            avg_misses=(
                'l1d_misses (actual)',
                'mean'
            ),
            avg_loads=(
                'l1d_loads (actual)',
                'mean'
            )
        )
        .reset_index()
    )

    # =========================================================
    # Display data
    # =========================================================

    print("\n========================================")
    print("TILING ANALYSIS")
    print("========================================")

    print("\nTiling Results:")
    print(tile_df)

    print("\nAverage Results by Tile Size:")
    print(avg_tile)

    # =========================================================
    # Plotting
    # =========================================================

    sns.set_theme(style="whitegrid")

    fig, axes = plt.subplots(
        2,
        3,
        figsize=(21, 12)
    )

    # =========================================================
    # Automatically determine orders
    # =========================================================

    tile_order = sorted(
        tile_df['tile_size'].unique()
    )

    matrix_order = sorted(
        tile_df['matrix_size'].unique()
    )

    # =========================================================
    # Convert categorical columns only for bar plots
    # =========================================================

    plot_df = tile_df.copy()

    plot_df['matrix_size'] = (
        plot_df['matrix_size'].astype(str)
    )

    plot_df['tile_size'] = (
        plot_df['tile_size'].astype(str)
    )

    tile_order_str = [
        str(x) for x in tile_order
    ]

    matrix_order_str = [
        str(x) for x in matrix_order
    ]

    # =========================================================
    # 1. GROUPED BARPLOT - SPEEDUP
    # =========================================================

    sns.barplot(
        data=plot_df,
        x='matrix_size',
        y='speedup',
        hue='tile_size',
        hue_order=tile_order_str,
        order=matrix_order_str,
        ax=axes[0, 0]
    )

    axes[0, 0].set_title(
        'Speedup vs Matrix Size',
        fontweight='bold'
    )

    axes[0, 0].set_xlabel('Matrix Size')
    axes[0, 0].set_ylabel('Speedup')

    axes[0, 0].legend(
        title='Tile Size',
        fontsize=8
    )

    # =========================================================
    # 2. GROUPED BARPLOT - L1-D MPKI
    # =========================================================

    sns.barplot(
        data=plot_df,
        x='matrix_size',
        y='mpki',
        hue='tile_size',
        hue_order=tile_order_str,
        order=matrix_order_str,
        ax=axes[0, 1]
    )

    axes[0, 1].set_title(
        'L1-D MPKI vs Matrix Size',
        fontweight='bold'
    )

    axes[0, 1].set_xlabel('Matrix Size')
    axes[0, 1].set_ylabel('L1-D MPKI')

    axes[0, 1].legend(
        title='Tile Size',
        fontsize=8
    )

    # =========================================================
    # 3. SPEEDUP HEATMAP
    # =========================================================

    speedup_heatmap = tile_df.pivot(
        index='tile_size',
        columns='matrix_size',
        values='speedup'
    )

    speedup_heatmap = speedup_heatmap.reindex(
        index=tile_order,
        columns=matrix_order
    )

    sns.heatmap(
        speedup_heatmap,
        annot=True,
        fmt='.2f',
        cmap='RdYlGn',
        center=1.0,
        ax=axes[0, 2]
    )

    axes[0, 2].set_title(
        'Speedup Heatmap',
        fontweight='bold'
    )

    axes[0, 2].set_xlabel('Matrix Size')
    axes[0, 2].set_ylabel('Tile Size')

    # =========================================================
    # 4. L1-D MISS RATE HEATMAP
    # =========================================================

    miss_rate_heatmap = tile_df.pivot(
        index='tile_size',
        columns='matrix_size',
        values='miss_rate'
    )

    miss_rate_heatmap = miss_rate_heatmap.reindex(
        index=tile_order,
        columns=matrix_order
    )

    # Convert to percentage for display
    miss_rate_heatmap = miss_rate_heatmap * 100

    sns.heatmap(
        miss_rate_heatmap,
        annot=True,
        fmt='.3f',
        cmap='RdYlGn_r',
        ax=axes[1, 0],
        cbar_kws={
            'label': 'L1-D Miss Rate (%)'
        }
    )

    axes[1, 0].set_title(
        'L1-D Miss Rate',
        fontweight='bold'
    )

    axes[1, 0].set_xlabel('Matrix Size')
    axes[1, 0].set_ylabel('Tile Size')

    # =========================================================
    # 5. AVERAGE SPEEDUP BY TILE SIZE
    # =========================================================

    sns.barplot(
        data=avg_tile,
        x='tile_size',
        y='avg_speedup',
        order=tile_order,
        palette=list(comparison_colors(avg_tile['avg_speedup'], center=1.0)),
        hue='tile_size',
        legend=False,
        ax=axes[1, 1]
    )

    axes[1, 1].set_title(
        'Average Speedup by Tile Size',
        fontweight='bold'
    )

    axes[1, 1].set_xlabel('Tile Size')
    axes[1, 1].set_ylabel('Average Speedup')

    for container in axes[1, 1].containers:
        axes[1, 1].bar_label(
            container,
            fmt='%.2fx',
            padding=3
        )

    # =========================================================
    # 6. TILE SIZE vs SPEEDUP
    # =========================================================

    sns.lineplot(
        data=tile_df,
        x='matrix_size',
        y='speedup',
        hue='tile_size',
        hue_order=tile_order,
        marker='o',
        ax=axes[1, 2]
    )

    axes[1, 2].set_title(
        'Effect of Tile Size on Performance',
        fontweight='bold'
    )

    axes[1, 2].set_xlabel('Matrix Size')
    axes[1, 2].set_ylabel('Speedup')
    
    axes[1, 2].set_xticks(matrix_order)

    axes[1, 2].set_xticklabels(
        matrix_order
    )

    axes[1, 2].legend(
        title='Tile Size',
        bbox_to_anchor=(1.02, 1),
        loc='upper left',
        fontsize=8
    )

    # =========================================================
    # Format axes
    # =========================================================

    for ax in axes.flat:
        ax.tick_params(
            axis='x',
            rotation=45
        )

    # =========================================================
    # Overall title
    # =========================================================

    fig.suptitle(
        'Performance Analysis of Tiling',
        fontsize=20,
        fontweight='bold'
    )

    # =========================================================
    # Layout
    # =========================================================

    plt.tight_layout(
        w_pad=3.0,
        h_pad=3.0,
        rect=(0, 0, 1, 0.96)
    )
    
    output_path = Path(__file__).resolve().parents[2] / 'results' / 'tiling_analysis.png'
    fig.savefig(output_path, dpi=300, bbox_inches='tight')