import numpy as np
from matplotlib import pyplot as plt
from matplotlib.colors import TwoSlopeNorm
from matplotlib.collections import LineCollection


GOOD_IS_GREEN = 'RdYlGn'


def comparison_colors(
    values,
    center=0.0,
    lower_is_better=False,
    baseline=None
):
    values = np.asarray(values, dtype=float)
    if baseline is None:
        scores = values - center
    else:
        scores = values - np.asarray(baseline, dtype=float)

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


def comparison_line(
    ax,
    x,
    values,
    baseline,
    label,
    marker='o',
    lower_is_better=False,
    linewidth=2.5
):
    x = np.asarray(x, dtype=float)
    values = np.asarray(values, dtype=float)
    colors = comparison_colors(
        values,
        baseline=baseline,
        lower_is_better=lower_is_better
    )

    points = np.column_stack((x, values))
    segments = np.stack((points[:-1], points[1:]), axis=1)
    if len(segments):
        ax.add_collection(
            LineCollection(
                segments,
                colors=colors[:-1],
                linewidths=linewidth,
                zorder=2
            )
        )

    ax.scatter(
        x,
        values,
        color=colors,
        marker=marker,
        s=55,
        label=label,
        zorder=3
    )
    ax.autoscale_view()
