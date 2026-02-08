from __future__ import annotations

import math

from ._core import (
    __doc__,
    __version__,
    _MarketData,
    DeadCatBounce,
    Drop,
    Earnings,
    GBM,
    InverseDeadCatBounce,
    JumpDiffusion,
    MeanReversion,
    Momentum,
    RandomWalk,
    RegimeAssignment,
    Regime,
    SineWave,
    Spike,
    TrendingMeanReversion,
)
from .presets import (
    BearQuiet,
    BearVolatile,
    BullQuiet,
    BullVolatile,
    ChopZone,
    Crisis,
    DisbeliefMomentum,
    FrenzyZone,
    SidewaysQuiet,
    Transition,
)


class _NanAwareList(list):
    """A list subclass where equality treats NaN values as equal."""

    def __eq__(self, other):
        if not isinstance(other, list):
            return NotImplemented
        if len(self) != len(other):
            return False
        for a, b in zip(self, other):
            if isinstance(a, float) and isinstance(b, float):
                if math.isnan(a) and math.isnan(b):
                    continue
            if a != b:
                return False
        return True

    def __getitem__(self, key):
        result = super().__getitem__(key)
        if isinstance(key, slice):
            return _NanAwareList(result)
        return result


def _wrap(data):
    """Wrap a list or tuple-of-lists with NaN-aware equality."""
    if isinstance(data, tuple):
        return tuple(_NanAwareList(item) for item in data)
    return _NanAwareList(data)


class _MarketDataWrapper:
    """Wrapper around _MarketData that returns NaN-aware lists from indicators."""

    def __init__(self, md):
        self._md = md

    def __getattr__(self, name):
        attr = getattr(self._md, name)
        # Wrap indicator methods that may return NaN values
        _indicator_methods = {
            "getBuySMA", "getSellSMA",
            "getBuyEMA", "getSellEMA",
            "getBuyRSI", "getSellRSI",
            "getBuyMACD", "getSellMACD",
            "getBuyBollingerBands", "getSellBollingerBands",
            "getATR",
        }
        if name in _indicator_methods and callable(attr):
            def wrapper(*args, **kwargs):
                return _wrap(attr(*args, **kwargs))
            return wrapper
        return attr


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
    return _MarketDataWrapper(
        _MarketData(start_buy_price, start_sell_price, assignments, seed),
    )


__all__ = [
    "__doc__",
    "__version__",
    "MarketData",
    "DeadCatBounce",
    "Drop",
    "Earnings",
    "GBM",
    "InverseDeadCatBounce",
    "JumpDiffusion",
    "MeanReversion",
    "Momentum",
    "RandomWalk",
    "RegimeAssignment",
    "Regime",
    "SineWave",
    "Spike",
    "TrendingMeanReversion",
    "BullQuiet",
    "BullVolatile",
    "BearQuiet",
    "BearVolatile",
    "SidewaysQuiet",
    "Crisis",
    "DisbeliefMomentum",
    "FrenzyZone",
    "ChopZone",
    "Transition",
]
