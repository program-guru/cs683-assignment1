import pandas as pd
import os


# ============================================================
# Configuration
# ============================================================

DATA_DIR = os.path.abspath(
    os.path.join(
        os.path.dirname(__file__),
        '..',
        'results'
    )
)


# ============================================================
# Load Data
# ============================================================

def load_data():

    naive_df = pd.read_csv(
        os.path.join(DATA_DIR, 'naive_results.csv')
    )

    reorder_df = pd.read_csv(
        os.path.join(DATA_DIR, 'reorder_results.csv')
    )

    simd_df = pd.read_csv(
        os.path.join(DATA_DIR, 'simd_results.csv')
    )

    tile_df = pd.read_csv(
        os.path.join(DATA_DIR, 'tile_results.csv')
    )

    unroll_df = pd.read_csv(
        os.path.join(DATA_DIR, 'unroll_results.csv')
    )

    return (
        naive_df,
        reorder_df,
        simd_df,
        tile_df,
        unroll_df
    )


# ============================================================
# Preprocess Data
# ============================================================

def preprocess_data():

    (
        naive_df,
        reorder_df,
        simd_df,
        tile_df,
        unroll_df
    ) = load_data()


    # --------------------------------------------------------
    # Remove speedup from Naive
    # --------------------------------------------------------

    if 'speedup' in naive_df.columns:
        naive_df.drop(
            columns='speedup',
            inplace=True
        )


    # --------------------------------------------------------
    # Rename common columns
    # --------------------------------------------------------

    all_dfs = {
        'naive': naive_df,
        'reorder': reorder_df,
        'simd': simd_df,
        'tile': tile_df,
        'unroll': unroll_df
    }

    for name, df in all_dfs.items():

        df.rename(
            columns={
                'instructions':
                    'instructions (combined)',

                'l1d_misses':
                    'l1d_misses (combined)',

                'l1d_loads':
                    'l1d_loads (combined)'
            },
            inplace=True
        )


    # --------------------------------------------------------
    # Naive MPKI
    # --------------------------------------------------------

    naive_df['mpki'] = (
        naive_df['l1d_misses (combined)'] * 1000
    ) / naive_df['instructions (combined)']

    naive_df['miss_rate'] = (
        naive_df['l1d_misses (combined)']
        .div(naive_df['l1d_loads (combined)'])
        .where(naive_df['l1d_loads (combined)'] > 0, 0.0)
    )

    # --------------------------------------------------------
    # Naive lookup maps
    # --------------------------------------------------------

    naive_inst_map = (
        naive_df
        .set_index('matrix_size')
        ['instructions (combined)']
    )

    naive_misses_map = (
        naive_df
        .set_index('matrix_size')
        ['l1d_misses (combined)']
    )

    naive_loads_map = (
        naive_df
        .set_index('matrix_size')
        ['l1d_loads (combined)']
    )


    # --------------------------------------------------------
    # Process optimized implementations
    # --------------------------------------------------------

    optimized_dfs = [
        reorder_df,
        simd_df,
        tile_df,
        unroll_df
    ]

    for df in optimized_dfs:

        # Actual instructions
        df['instructions (actual)'] = (
            df['instructions (combined)']
            - df['matrix_size'].map(naive_inst_map)
        )
        df['instructions (actual)'] = df['instructions (actual)'].clip(lower=1)

        # Actual L1-D misses
        df['l1d_misses (actual)'] = (
            df['l1d_misses (combined)']
            - df['matrix_size'].map(naive_misses_map)
        )
        df['l1d_misses (actual)'] = df['l1d_misses (actual)'].clip(lower=0)

        # Actual L1-D loads
        df['l1d_loads (actual)'] = (
            df['l1d_loads (combined)']
            - df['matrix_size'].map(naive_loads_map)
        )
        df['l1d_loads (actual)'] = df['l1d_loads (actual)'].clip(lower=0)

        # MPKI
        df['mpki'] = (
            df['l1d_misses (actual)'] * 1000
        ) / df['instructions (actual)']
        df['mpki'] = df['mpki'].clip(lower=0)

        # L1-D miss rate
        df['miss_rate'] = (
            df['l1d_misses (actual)']
            .div(df['l1d_loads (actual)'])
            .where(df['l1d_loads (actual)'] > 0, 0.0)
        )


    return {
        'naive': naive_df,
        'reorder': reorder_df,
        'simd': simd_df,
        'tile': tile_df,
        'unroll': unroll_df
    }