import pandas as pd
import os
import matplotlib.pyplot as plt
import seaborn as sns

# Set directory path
data_dir = '/home/shivam/Documents/IIT Bombay/CS 683/cs683-assignment1/task2/results'

# Import all files into separate DataFrames
naive_df = pd.read_csv(os.path.join(data_dir, 'naive_results.csv'))
prefetch_df = pd.read_csv(os.path.join(data_dir, 'prefetch_results.csv'))
simd_df = pd.read_csv(os.path.join(data_dir, 'simd_results.csv'))

# Remove the speedup column from the naive results
naive_df.drop(columns='speedup', inplace=True)

# Group dataframes in a dictionary for easy iteration
all_dfs = {
    'naive': naive_df,
    'prefetch': prefetch_df,
    'simd': simd_df,
    
}

for name, df in all_dfs.items():
    # Rename columns to include "(combined)"
    df.rename(columns={
        'instructions': 'instructions (combined)',
        'l1d_misses': 'l1d_misses (combined)'
    }, inplace=True)
    

naive_inst_map = naive_df.set_index('matrix_size')['instructions (combined)']
naive_misses_map = naive_df.set_index('matrix_size')['l1d_misses (combined)']

# Group optimized dataframes to apply Actual and MPKI logic
optimized_dfs = [prefetch_df, simd_df]

for df in optimized_dfs:
    # Add Instructions (Actual) by looking up naive values for the matching Matrix Size
    df['instructions (actual)'] = df['instructions (combined)'] - df['matrix_size'].map(naive_inst_map)
    
    # Add L1D Misses (Actual)
    df['l1d_misses (actual)'] = df['l1d_misses (combined)'] - df['matrix_size'].map(naive_misses_map)
    
    # Add MPKI column
    df['mpki'] = (df['l1d_misses (actual)'] * 1000) / df['instructions (actual)']
    

# Set a clean visual theme
sns.set_theme(style="whitegrid")

# Create a layout with 2 rows and 2 columns
fig, axes = plt.subplots(2, 2, figsize=(15, 12))
heatmap_data = prefetch_df.pivot_table(
    index="cache_level",
    columns="prefetch_distance",
    values="speedup",
    aggfunc="mean",
)

sns.heatmap(heatmap_data, annot=True, fmt=".2f", cmap="YlGnBu", cbar_kws={'label': 'Speedup'})
plt.title("SGEMM Speedup: Prefetch Distance vs. Cache Level")
plt.xlabel("Prefetch Distance")
plt.ylabel("Cache Fill Level (_MM_HINT)")
plt.tight_layout()

fig, ax1 = plt.subplots(figsize=(10, 6))

# Primary Axis: L1D Cache Misses
sns.barplot(
    data=prefetch_df,
    x="prefetch_distance",
    y="l1d_misses(actual)",
    hue="cache_level",
    ax=ax1,
    alpha=0.7,
)

ax1.set_ylabel("L1D Misses", color="navy")
ax1.set_xlabel("Prefetch Distance")

# Overlay Secondary Axis: Mean Speedup Line
ax2 = ax1.twinx()
speedup_avg = (
    prefetch_df.groupby("prefetch_distance")["speedup"].mean().reset_index()
)
sns.lineplot(
    data=speedup_avg,
    x=range(len(speedup_avg)),
    y="speedup",
    color="crimson",
    marker="D",
    linewidth=2.5,
    ax=ax2,
    label="Avg Speedup",
)
ax2.set_ylabel("Average Speedup", color="crimson")
ax2.grid(False)

plt.title("L1D Cache Misses and Impact on Speedup")
plt.tight_layout()


plt.show()