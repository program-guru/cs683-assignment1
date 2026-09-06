import numpy as np
from matplotlib import pyplot as plt
from matplotlib.colors import TwoSlopeNorm


GOOD_IS_GREEN = 'RdYlGn'


def comparison_colors(values, center=0.0, lower_is_better=False):
    values = np.asarray(values, dtype=float)
    scores = values - center

    if lower_is_better:
        scores = -scores

    max_deviation = np.max(np.abs(scores)) if scores.size else 1.0
    if max_deviation == 0:
        max_deviation = 1.0

    norm = TwoSlopeNorm(
        vmin=-max_deviation,
        vcenter=0.0,
        vmax=max_deviation
    )

    return plt.get_cmap(GOOD_IS_GREEN)(norm(scores))
