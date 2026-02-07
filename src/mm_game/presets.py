from __future__ import annotations

from ._core import (
    GBM,
    JumpDiffusion,
    MeanReversion,
    Momentum,
    TrendingMeanReversion,
)


def BullQuiet(scale=1.0):
    """Steady uptrend, low volatility."""
    return GBM(mu=0.001, sigma=0.005 * scale)


def BullVolatile(scale=1.0):
    """Strong uptrend, large price swings."""
    return GBM(mu=0.003, sigma=0.04 * scale)


def BearQuiet(scale=1.0):
    """Steady downtrend, low volatility."""
    return GBM(mu=-0.001, sigma=0.005 * scale)


def BearVolatile(scale=1.0):
    """Rapid downtrend, high volatility."""
    return GBM(mu=-0.003, sigma=0.04 * scale)


def SidewaysQuiet(mu=100.0, scale=1.0):
    """Range-bound price action, low volatility."""
    return MeanReversion(mu=mu, theta=0.3, sigma=0.2 * scale)


def Crisis(scale=1.0):
    """Extreme volatility with severe drawdowns and negative jumps."""
    return JumpDiffusion(
        mu=-0.005, sigma=0.06 * scale, jump_intensity=0.3, jump_size=-0.08
    )


def DisbeliefMomentum(mu=100.0, scale=1.0):
    """Slow grind up along a rising floor, pullbacks snap back."""
    return TrendingMeanReversion(
        mu=mu, drift=0.3, theta=0.15, sigma=0.5 * scale
    )


def FrenzyZone(scale=1.0):
    """Parabolic acceleration, euphoria, positive feedback loops."""
    return Momentum(mu=0.003, sigma=0.04 * scale, momentum=0.5)


def ChopZone(mu=100.0, scale=1.0):
    """Whipsaw price action, rapid mean-reverting noise."""
    return MeanReversion(mu=mu, theta=0.5, sigma=0.8 * scale)


def Transition(scale=1.0):
    """Directionless, moderate volatility, regime shift feel."""
    return GBM(mu=0.0, sigma=0.03 * scale)
