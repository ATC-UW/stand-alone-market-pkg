from __future__ import annotations

from ._core import (
    __doc__,
    __version__,
    _MarketData,
    Drop,
    GBM,
    JumpDiffusion,
    MeanReversion,
    RandomWalk,
    RegimeAssignment,
    Regime,
    SineWave,
    Spike,
)


def MarketData(start_buy_price, start_sell_price, regimes, seed=None):
    """Create a MarketData price simulator with configurable regimes.

    Args:
        start_buy_price: Initial buy price.
        start_sell_price: Initial sell price.
        regimes: List of (regime, day_range) tuples.
        seed: Optional RNG seed for reproducibility.
    """
    assignments = []
    for regime, days in regimes:
        assignments.append(RegimeAssignment(regime, days.start, days.stop))
    return _MarketData(start_buy_price, start_sell_price, assignments, seed)


__all__ = [
    "__doc__",
    "__version__",
    "MarketData",
    "Drop",
    "GBM",
    "JumpDiffusion",
    "MeanReversion",
    "RandomWalk",
    "RegimeAssignment",
    "Regime",
    "SineWave",
    "Spike",
]
