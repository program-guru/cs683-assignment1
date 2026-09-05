from Process import preprocess_data

from visualizations.reorder import plot_reorder
from visualizations.simd import plot_simd
from visualizations.tiling import plot_tile
from visualizations.unroll import plot_unroll


# ============================================================
# Preprocess Everything
# ============================================================

data = preprocess_data()


# ============================================================
# Extract DataFrames
# ============================================================

naive_df = data['naive']
reorder_df = data['reorder']
simd_df = data['simd']
tile_df = data['tile']
unroll_df = data['unroll']


# ============================================================
# Display Preprocessed Data
# ============================================================

print("\n========================================")
print("NAIVE")
print("========================================")

print(naive_df)


print("\n========================================")
print("REORDER")
print("========================================")

print(reorder_df)


print("\n========================================")
print("SIMD")
print("========================================")

print(simd_df)


print("\n========================================")
print("TILING")
print("========================================")

print(tile_df)


print("\n========================================")
print("UNROLL")
print("========================================")

print(unroll_df)


# ============================================================
# Generate Visualizations
# ============================================================
plot_reorder(
    naive_df,
    reorder_df
)
plot_unroll(
    naive_df,
    unroll_df
)
plot_tile(
    naive_df,
    tile_df
)
plot_simd(
    naive_df,
    simd_df
)