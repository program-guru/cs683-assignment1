import pandas as pd
import os
import matplotlib.pyplot as plt
import seaborn as sns

# Set directory path
data_dir = '/home/shivam/Documents/IIT Bombay/CS 683/cs683-assignment1/task1/results'

# Import all files into separate DataFrames
naive_df = pd.read_csv(os.path.join(data_dir, 'naive_results.csv'))
reorder_df = pd.read_csv(os.path.join(data_dir, 'reorder_results.csv'))
simd_df = pd.read_csv(os.path.join(data_dir, 'simd_results.csv'))
tile_df = pd.read_csv(os.path.join(data_dir, 'tile_results.csv'))
unroll_df = pd.read_csv(os.path.join(data_dir, 'unroll_results.csv'))

# Remove the speedup column from the naive results
naive_df.drop(columns='speedup', inplace=True)

# Group dataframes in a dictionary for easy iteration
all_dfs = {
    'naive': naive_df,
    'reorder': reorder_df,
    'simd': simd_df,
    'tile': tile_df,
    'unroll': unroll_df
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
optimized_dfs = [reorder_df, simd_df, tile_df, unroll_df]

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

# 1. Matrix Size (x-axis) vs Speedup (y-axis) grouped by Tile Size
sns.barplot(
    data=tile_df, 
    x='matrix_size', 
    y='speedup', 
    hue='tile_size', 
    ax=axes[0, 0],
    palette="viridis"
)
axes[0, 0].axhline(y=1, color='red', linestyle='--', linewidth=2, label='Naive Baseline (y=1)')
axes[0, 0].set_title('Speedup by Matrix Size (Grouped by Tile Size)', fontweight='bold')
axes[0, 0].set_xlabel('Matrix Size')
axes[0, 0].set_ylabel('Speedup')
axes[0, 0].legend(title='Tile Size')

# 2. Tile Size (x-axis) vs Speedup (y-axis) grouped by Matrix Size
sns.barplot(
    data=tile_df, 
    x='tile_size', 
    y='speedup', 
    hue='matrix_size', 
    ax=axes[0, 1],
    palette="magma"
)
axes[0, 1].axhline(y=1, color='red', linestyle='--', linewidth=2, label='Naive Baseline (y=1)')
axes[0, 1].set_title('Speedup by Tile Size (Grouped by Matrix Size)', fontweight='bold')
axes[0, 1].set_xlabel('Tile Size')
axes[0, 1].set_ylabel('Speedup')
axes[0, 1].legend(title='Matrix Size')

# 3. Table/Heatmap: Matrix Size (rows) vs Tile Size (columns) showing Speedup
# Pivot the dataframe to create a 2D matrix layout
heatmap_data = tile_df.pivot(index='matrix_size', columns='tile_size', values='speedup')

# Plot heatmap using a Red-Yellow-Green colormap centered exactly at 1.0
sns.heatmap(
    heatmap_data, 
    ax=axes[1, 0], 
    cmap="RdYlGn",    # Dark Red < 1, Yellow = 1, Dark Green > 1
    center=1.0, 
    annot=True,       # Display the numeric values in the cells
    fmt=".2f",        # Format to 2 decimal places
    linewidths=.5,
    cbar_kws={'label': 'Speedup'}
)
axes[1, 0].set_title('Speedup Heatmap Matrix (>1 Green, <1 Red)', fontweight='bold')
axes[1, 0].set_xlabel('Tile Size')
axes[1, 0].set_ylabel('Matrix Size')

# Hide the unused 4th subplot in the bottom right corner
fig.delaxes(axes[1, 1])

# Adjust spacing and render
plt.tight_layout(w_pad=3.0, h_pad=3.0)
plt.show()