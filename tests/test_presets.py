from __future__ import annotations

from mm_game import (
    MarketData,
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

SEED = 42
NUM_DAYS = 100


class TestPresetsWork:
    """All presets produce valid prices without crashing."""

    def test_bull_quiet(self):
        md = MarketData(100.0, 99.0, [(BullQuiet(), range(0, NUM_DAYS))], seed=SEED)
        assert all(p > 0 for p in md.getBuyPrices())
        assert all(p > 0 for p in md.getSellPrices())

    def test_bull_volatile(self):
        md = MarketData(100.0, 99.0, [(BullVolatile(), range(0, NUM_DAYS))], seed=SEED)
        assert all(p > 0 for p in md.getBuyPrices())
        assert all(p > 0 for p in md.getSellPrices())

    def test_bear_quiet(self):
        md = MarketData(100.0, 99.0, [(BearQuiet(), range(0, NUM_DAYS))], seed=SEED)
        assert all(p > 0 for p in md.getBuyPrices())
        assert all(p > 0 for p in md.getSellPrices())

    def test_bear_volatile(self):
        md = MarketData(100.0, 99.0, [(BearVolatile(), range(0, NUM_DAYS))], seed=SEED)
        assert all(p > 0 for p in md.getBuyPrices())
        assert all(p > 0 for p in md.getSellPrices())

    def test_sideways_quiet(self):
        md = MarketData(100.0, 99.0, [(SidewaysQuiet(), range(0, NUM_DAYS))], seed=SEED)
        assert all(p > 0 for p in md.getBuyPrices())
        assert all(p > 0 for p in md.getSellPrices())

    def test_crisis(self):
        md = MarketData(100.0, 99.0, [(Crisis(), range(0, NUM_DAYS))], seed=SEED)
        assert all(p > 0 for p in md.getBuyPrices())
        assert all(p > 0 for p in md.getSellPrices())

    def test_disbelief_momentum(self):
        md = MarketData(100.0, 99.0, [(DisbeliefMomentum(), range(0, NUM_DAYS))], seed=SEED)
        assert all(p > 0 for p in md.getBuyPrices())
        assert all(p > 0 for p in md.getSellPrices())

    def test_frenzy_zone(self):
        md = MarketData(100.0, 99.0, [(FrenzyZone(), range(0, NUM_DAYS))], seed=SEED)
        assert all(p > 0 for p in md.getBuyPrices())
        assert all(p > 0 for p in md.getSellPrices())

    def test_chop_zone(self):
        md = MarketData(100.0, 99.0, [(ChopZone(), range(0, NUM_DAYS))], seed=SEED)
        assert all(p > 0 for p in md.getBuyPrices())
        assert all(p > 0 for p in md.getSellPrices())

    def test_transition(self):
        md = MarketData(100.0, 99.0, [(Transition(), range(0, NUM_DAYS))], seed=SEED)
        assert all(p > 0 for p in md.getBuyPrices())
        assert all(p > 0 for p in md.getSellPrices())


class TestPresetBehavior:
    """Presets produce expected directional behavior."""

    def test_bull_quiet_trends_up(self):
        md = MarketData(100.0, 99.0, [(BullQuiet(), range(0, NUM_DAYS))], seed=SEED)
        buys = md.getBuyPrices()
        assert buys[-1] > buys[0]

    def test_bear_quiet_trends_down(self):
        md = MarketData(100.0, 99.0, [(BearQuiet(), range(0, NUM_DAYS))], seed=SEED)
        buys = md.getBuyPrices()
        assert buys[-1] < buys[0]

    def test_sideways_quiet_stays_near_mu(self):
        mu = 100.0
        md = MarketData(100.0, 99.0, [(SidewaysQuiet(mu=mu), range(0, 200))], seed=SEED)
        buys = md.getBuyPrices()
        assert abs(buys[-1] - mu) < 20.0

    def test_crisis_trends_down(self):
        md = MarketData(100.0, 99.0, [(Crisis(), range(0, NUM_DAYS))], seed=SEED)
        buys = md.getBuyPrices()
        assert buys[-1] < buys[0]

    def test_frenzy_zone_trends_up(self):
        md = MarketData(100.0, 99.0, [(FrenzyZone(), range(0, NUM_DAYS))], seed=SEED)
        buys = md.getBuyPrices()
        assert buys[-1] > buys[0]


class TestPresetScale:
    """Scale parameter adjusts volatility."""

    def test_higher_scale_more_volatile(self):
        md_low = MarketData(100.0, 99.0, [(BullQuiet(scale=0.5), range(0, NUM_DAYS))], seed=SEED)
        md_high = MarketData(100.0, 99.0, [(BullQuiet(scale=2.0), range(0, NUM_DAYS))], seed=SEED)

        prices_low = md_low.getBuyPrices()
        prices_high = md_high.getBuyPrices()

        returns_low = [
            (prices_low[i] - prices_low[i - 1]) / prices_low[i - 1]
            for i in range(1, len(prices_low))
        ]
        returns_high = [
            (prices_high[i] - prices_high[i - 1]) / prices_high[i - 1]
            for i in range(1, len(prices_high))
        ]
        var_low = sum(r * r for r in returns_low) / len(returns_low)
        var_high = sum(r * r for r in returns_high) / len(returns_high)

        assert var_high > var_low


class TestPresetMuOverride:
    """Mean-reversion presets respect mu override."""

    def test_sideways_quiet_mu_override(self):
        mu = 50.0
        md = MarketData(50.0, 49.0, [(SidewaysQuiet(mu=mu), range(0, 200))], seed=SEED)
        buys = md.getBuyPrices()
        assert abs(buys[-1] - mu) < 20.0

    def test_chop_zone_mu_override(self):
        mu = 200.0
        md = MarketData(200.0, 199.0, [(ChopZone(mu=mu), range(0, 200))], seed=SEED)
        buys = md.getBuyPrices()
        assert abs(buys[-1] - mu) < 50.0


class TestPresetMultiRegime:
    """Presets work in multi-regime configurations."""

    def test_bull_to_crisis(self):
        regimes = [
            (BullQuiet(), range(0, 50)),
            (Crisis(), range(50, 100)),
        ]
        md = MarketData(100.0, 99.0, regimes, seed=SEED)
        buys = md.getBuyPrices()
        assert buys[49] > buys[0]
        assert buys[99] < buys[50]
