import pandas as pd
import os
import matplotlib.pyplot as plt
import seaborn as sns

# ============================================================
# Configuration
# ============================================================

data_dir = '../results'

# Load naive results
naive_df = pd.read_csv(
    os.path.join(data_dir, 'naive_results.csv')
)

# Sort by matrix size
naive_df = naive_df.sort_values('matrix_size')

# ============================================================
# Calculate Metrics
# ============================================================

# L1-D MPKI
naive_df['mpki'] = (
    naive_df['l1d_misses'] * 1000
) / naive_df['instructions']

# Total number of output elements
naive_df['output_elements'] = (
    naive_df['matrix_size'] ** 2
)

# Instructions per output element
naive_df['instructions_per_output'] = (
    naive_df['instructions'] /
    naive_df['output_elements']
)

# L1-D miss rate
naive_df['l1d_miss_rate'] = (
    naive_df['l1d_misses'] /
    naive_df['instructions']
) * 100

# ============================================================
# Display data
# ============================================================

print("\nNaive Results:")
print(naive_df)

# ============================================================
# Plotting style
# ============================================================

sns.set_theme(style="whitegrid")

# ONE figure containing 4 subplots
fig, axes = plt.subplots(
    2, 2,
    figsize=(15, 12)
)

# Actual matrix sizes
matrix_sizes = sorted(
    naive_df['matrix_size'].unique()
)

# ============================================================
# 1. Matrix Size vs MPKI
# ============================================================

sns.lineplot(
    data=naive_df,
    x='matrix_size',
    y='mpki',
    marker='o',
    linewidth=2.5,
    ax=axes[0, 0]
)

axes[0, 0].set_title(
    'Naive: L1-D Cache MPKI vs Matrix Size',
    fontweight='bold'
)

axes[0, 0].set_xlabel('Matrix Size')
axes[0, 0].set_ylabel('L1-D MPKI')

axes[0, 0].set_xticks(matrix_sizes)


# ============================================================
# 2. Normalized Growth
# ============================================================

base_instructions = naive_df['instructions'].iloc[0]
base_misses = naive_df['l1d_misses'].iloc[0]

naive_df['instruction_growth'] = (
    naive_df['instructions'] /
    base_instructions
)

naive_df['miss_growth'] = (
    naive_df['l1d_misses'] /
    base_misses
)

axes[0, 1].plot(
    naive_df['matrix_size'],
    naive_df['instruction_growth'],
    marker='o',
    linewidth=2.5,
    label='Instructions'
)

axes[0, 1].plot(
    naive_df['matrix_size'],
    naive_df['miss_growth'],
    marker='s',
    linewidth=2.5,
    label='L1-D Misses'
)

axes[0, 1].set_title(
    'Naive: Normalized Growth',
    fontweight='bold'
)

axes[0, 1].set_xlabel('Matrix Size')
axes[0, 1].set_ylabel(
    'Growth Relative to Smallest Matrix'
)

axes[0, 1].set_xticks(matrix_sizes)
axes[0, 1].legend()


sns.barplot(
    data=naive_df,
    x='matrix_size',
    y='speedup',
    ax=axes[1, 0]
)

axes[1, 0].set_title(
    'Naive: Speedup vs Matrix Size',
    fontweight='bold'
)

axes[1, 0].set_xlabel('Matrix Size')
axes[1, 0].set_ylabel('Speedup')

# Add speedup values above bars
for container in axes[1, 0].containers:
    axes[1, 0].bar_label(
        container,
        fmt='%.2fx',
        padding=3
    )

# ============================================================
# 4. L1-D Cache Miss Rate
# ============================================================

sns.barplot(
    data=naive_df,
    x='matrix_size',
    y='l1d_miss_rate',
    ax=axes[1, 1]
)

axes[1, 1].set_title(
    'Naive: L1-D Cache Miss Rate',
    fontweight='bold'
)

axes[1, 1].set_xlabel('Matrix Size')
axes[1, 1].set_ylabel(
    'L1-D Miss Rate (%)'
)


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
    rect=[0, 0, 1, 0.96]
)

plt.show()