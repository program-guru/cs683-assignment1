import os
import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker
import seaborn as sns

# Set directory path
data_dir = '/home/shivam/Documents/IIT Bombay/CS 683/cs683-assignment1/task2/results'

# Import all files into separate DataFrames
naive_df = pd.read_csv(os.path.join(data_dir, 'naive_results.csv'))
prefetch_df = pd.read_csv(os.path.join(data_dir, 'prefetch_results.csv'))
simd_df = pd.read_csv(os.path.join(data_dir, 'simd_results.csv'))
optimized_df = pd.read_csv(os.path.join(data_dir, 'optimized_results.csv'))

# Data Preprocessing: Merge and subtract naive baseline
merged_df = prefetch_df.merge(
    naive_df[['matrix_size', 'naive_instructions', 'l1d_misses', 'l1d_loads']],
    on='matrix_size',
    suffixes=('', '_naive'),
)

merged_df['net_instructions'] = (
    merged_df['instructions'] - merged_df['naive_instructions']
)
merged_df['net_l1d_misses'] = (
    merged_df['l1d_misses'] - merged_df['l1d_misses_naive']
)
merged_df['l1d_miss_rate'] = (
    merged_df['net_l1d_misses'] / merged_df['l1d_loads']
) * 100

# Setup 2x2 Subplot Layout
sns.set_theme(style='whitegrid')

# Note: 3 rows and 2 columns so axes[2, 0] and axes[2, 1] are valid
fig, axes = plt.subplots(3, 2, figsize=(16, 17))

df_512 = merged_df[merged_df['matrix_size'] == 512]

# Subplot 1: Prefetch Distance vs. Speedup (N=512)
sns.lineplot(
    data=df_512,
    x='prefetch_distance',
    y='speedup',
    hue='cache_level',
    marker='o',
    linewidth=2.5,
    palette='Set1',
    ax=axes[0, 0],
)
axes[0, 0].set_xscale('log', base=2)
axes[0, 0].xaxis.set_major_formatter(ticker.ScalarFormatter())
axes[0, 0].set_xticks(sorted(df_512['prefetch_distance'].unique()))
axes[0, 0].set_title(
    'Prefetch Distance vs. Speedup (N=512)', fontsize=12, fontweight='bold'
)
axes[0, 0].set_xlabel('Prefetch Distance', fontweight='bold')
axes[0, 0].set_ylabel('Speedup', fontweight='bold')
axes[0, 0].legend(title='Cache Level', loc='best')

# Subplot 2: Prefetch Distance vs. L1 Miss Rate (%) (N=512)
sns.barplot(
    data=df_512,
    x='prefetch_distance',
    y='l1d_miss_rate',
    hue='cache_level',
    palette='Set1',
    ax=axes[0, 1],
)
axes[0, 1].set_title(
    'Prefetch Distance vs. L1 Cache Miss Rate (%) (N=512)',
    fontsize=12,
    fontweight='bold',
)
axes[0, 1].set_xlabel('Prefetch Distance', fontweight='bold')
axes[0, 1].set_ylabel('L1D Cache Miss Rate (%)', fontweight='bold')
axes[0, 1].legend(title='Cache Level', loc='best')

# Subplot 3: Matrix Size vs. Speedup (Prefetch Dist = 128)
CONSTANT_PREFETCH_DIST = 128
df_constant = merged_df[
    merged_df['prefetch_distance'] == CONSTANT_PREFETCH_DIST
]

sns.lineplot(
    data=df_constant,
    x='matrix_size',
    y='speedup',
    hue='cache_level',
    marker='^',
    linewidth=2.5,
    palette='Set1',
    ax=axes[1, 0],
)
axes[1, 0].set_xticks([256, 512, 752, 1024, 1256, 1504])
axes[1, 0].set_xlim(200, 1600)
axes[1, 0].set_title(
    f'Matrix Size vs. Speedup (Prefetch Dist = {CONSTANT_PREFETCH_DIST})',
    fontsize=12,
    fontweight='bold',
)
axes[1, 0].set_xlabel('Matrix Size (N)', fontweight='bold')
axes[1, 0].set_ylabel('Speedup', fontweight='bold')
axes[1, 0].legend(title='Cache Level', loc='best')

# Subplot 4: SGEMM Speedup Heatmap
heatmap_data = prefetch_df.pivot_table(
    index='cache_level',
    columns='prefetch_distance',
    values='speedup',
    aggfunc='mean',
)
sns.heatmap(
    heatmap_data,
    annot=True,
    fmt='.2f',
    cmap='YlGnBu',
    cbar_kws={'label': 'Speedup'},
    ax=axes[1, 1],
)
axes[1, 1].set_title(
    'SGEMM Speedup: Prefetch Distance vs. Cache Level',
    fontsize=12,
    fontweight='bold',
)
axes[1, 1].set_xlabel('Prefetch Distance', fontweight='bold')
axes[1, 1].set_ylabel('Cache Fill Level (_MM_HINT)', fontweight='bold')

# Subplot 5: Naive Execution Time vs. Matrix Size
sns.lineplot(
    data=naive_df,
    x='matrix_size',
    y='naive_time',
    marker='o',
    color='darkred',
    linewidth=2.5,
    ax=axes[2, 0],
)
axes[2, 0].set_title(
    'Naive Execution Time vs. Matrix Size', fontsize=12, fontweight='bold'
)
axes[2, 0].set_xlabel('Matrix Size (N)', fontweight='bold')
axes[2, 0].set_ylabel('Naive Time (s)', fontweight='bold')

# Subplot 6: Naive L1 Miss Rate (%) vs. Matrix Size
sns.lineplot(
    data=naive_df,
    x='matrix_size',
    y='naive_miss_rate',
    marker='s',
    color='teal',
    linewidth=2.5,
    ax=axes[2, 1],
)
axes[2, 1].set_title(
    'Naive L1 Cache Miss Rate (%) vs. Matrix Size',
    fontsize=12,
    fontweight='bold',
)
axes[2, 1].set_xlabel('Matrix Size (N)', fontweight='bold')
axes[2, 1].set_ylabel('L1D Miss Rate (%)', fontweight='bold')

plt.tight_layout()
plt.show()