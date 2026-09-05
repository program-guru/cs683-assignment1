import pandas as pd
import os
import matplotlib.pyplot as plt
import seaborn as sns

# ---------------------------------------------------------
# Load data
# ---------------------------------------------------------

data_dir = '../results'

tile_df = pd.read_csv(
    os.path.join(data_dir, 'tile_results.csv')
)

tile_df = tile_df.sort_values(
    ['matrix_size', 'tile_size']
)

# ---------------------------------------------------------
# Calculate L1-D MPKI
# ---------------------------------------------------------

tile_df['mpki'] = (
    tile_df['l1d_misses'] * 1000
) / tile_df['instructions']

print("\nTile sizes:")
print(sorted(tile_df['tile_size'].unique()))

print("\nTiling Results:")
print(tile_df)

# ---------------------------------------------------------
# Load Naive results
# ---------------------------------------------------------

naive_df = pd.read_csv(
    os.path.join(data_dir, 'naive_results.csv')
)

naive_df = naive_df.sort_values('matrix_size')

# ---------------------------------------------------------
# Map Naive cache misses
# ---------------------------------------------------------

naive_miss_map = dict(
    zip(
        naive_df['matrix_size'],
        naive_df['l1d_misses']
    )
)

tile_df['naive_misses'] = tile_df['matrix_size'].map(
    naive_miss_map
)

# ---------------------------------------------------------
# Calculate CACHE MISS INCREASE
# ---------------------------------------------------------

tile_df['miss_increase'] = (
    (
        tile_df['l1d_misses']
        - tile_df['naive_misses']
    )
    / tile_df['naive_misses']
) * 100

# ---------------------------------------------------------
# Average results
# ---------------------------------------------------------

avg_tile = (
    tile_df
    .groupby('tile_size')
    .agg(
        avg_speedup=('speedup', 'mean'),
        avg_mpki=('mpki', 'mean'),
        avg_misses=('l1d_misses', 'mean')
    )
    .reset_index()
)

print("\nAverage Results by Tile Size:")
print(avg_tile)

# ---------------------------------------------------------
# Plotting
# ---------------------------------------------------------

sns.set_theme(style="whitegrid")

fig, axes = plt.subplots(
    2, 3,
    figsize=(21, 12)
)

print("UNIQUE TILE SIZES:", tile_df['tile_size'].unique())
print("SORTED TILE SIZES:", sorted(tile_df['tile_size'].unique()))

# =========================================================
# 1. GROUPED BARPLOT - SPEEDUP
# =========================================================

plot_df = tile_df.copy()

plot_df['matrix_size'] = plot_df['matrix_size'].astype(str)
plot_df['tile_size'] = plot_df['tile_size'].astype(str)

tile_order = [
    '8', '16', '32', '64',
    '128', '256', '512', '1024'
]

sns.barplot(
    data=plot_df,
    x='matrix_size',
    y='speedup',
    hue='tile_size',
    hue_order=tile_order,
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
    hue_order=tile_order,
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

sns.heatmap(
    speedup_heatmap,
    annot=True,
    fmt='.2f',
    cmap='YlGnBu',
    ax=axes[0, 2]
)

axes[0, 2].set_title(
    'Speedup Heatmap',
    fontweight='bold'
)

axes[0, 2].set_xlabel('Matrix Size')
axes[0, 2].set_ylabel('Tile Size')

# =========================================================
# 4. CACHE MISS INCREASE HEATMAP
# =========================================================

miss_heatmap = tile_df.pivot(
    index='tile_size',
    columns='matrix_size',
    values='miss_increase'
)

sns.heatmap(
    miss_heatmap,
    annot=True,
    fmt='.1f',
    cmap='Reds',
    center=0,
    ax=axes[1, 0]
)

axes[1, 0].set_title(
    'L1-D Cache Miss Increase vs Naive',
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

plot6_df = tile_df.copy()

# Convert matrix size to strings for a clean categorical legend
plot6_df['matrix_size'] = plot6_df['matrix_size'].astype(str)


matrix_order = [
    '256','512','752', '1024','1256','1504','1752','2048'
]

sns.lineplot(
    data=plot6_df,
    x='tile_size',
    y='speedup',
    hue='matrix_size',
    hue_order=matrix_order,
    marker='o',
    ax=axes[1, 2]
)

axes[1, 2].set_title(
    'Effect of Tile Size on Performance',
    fontweight='bold'
)

axes[1, 2].set_xlabel('Tile Size')
axes[1, 2].set_ylabel('Speedup')

# Use logarithmic spacing because tile sizes double
axes[1, 2].set_xscale('log', base=2)

# Show actual tile sizes instead of 2^n
tile_sizes = sorted(tile_df['tile_size'].unique())

axes[1, 2].set_xticks(tile_sizes)
axes[1, 2].set_xticklabels(
    [str(x) for x in tile_sizes]
)

# Make matrix-size legend readable

axes[1, 2].legend(
    title='Matrix Size',
    bbox_to_anchor=(1.02, 1),
    loc='upper left',
    fontsize=8
)
# ---------------------------------------------------------
# Format axes
# ---------------------------------------------------------

for ax in axes.flat:
    ax.tick_params(
        axis='x',
        rotation=45
    )

# ---------------------------------------------------------
# Overall title
# ---------------------------------------------------------

fig.suptitle(
    'Performance Analysis of Tiling',
    fontsize=20,
    fontweight='bold'
)

plt.tight_layout(
    w_pad=3.0,
    h_pad=3.0,
    rect=[0, 0, 1, 0.96]
)


plt.show()