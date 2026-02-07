from __future__ import annotations

from mm_game import (
    MarketData,
    Drop,
    GBM,
    JumpDiffusion,
    MeanReversion,
    RandomWalk,
    SineWave,
    Spike,
)

SEED = 42
NUM_DAYS = 100


class TestReproducibility:
    def test_same_seed_same_prices(self):
        regimes = [(GBM(mu=0.0005, sigma=0.02), range(0, NUM_DAYS))]
        md1 = MarketData(100.0, 99.0, regimes, seed=SEED)
        md2 = MarketData(100.0, 99.0, regimes, seed=SEED)

        assert md1.getBuyPrices() == md2.getBuyPrices()
        assert md1.getSellPrices() == md2.getSellPrices()

    def test_different_seed_different_prices(self):
        regimes = [(GBM(mu=0.0005, sigma=0.02), range(0, NUM_DAYS))]
        md1 = MarketData(100.0, 99.0, regimes, seed=1)
        md2 = MarketData(100.0, 99.0, regimes, seed=2)

        assert md1.getBuyPrices() != md2.getBuyPrices()


class TestGBM:
    def test_prices_stay_positive(self):
        regimes = [(GBM(mu=0.0, sigma=0.02), range(0, NUM_DAYS))]
        md = MarketData(100.0, 99.0, regimes, seed=SEED)
        assert all(p > 0 for p in md.getBuyPrices())
        assert all(p > 0 for p in md.getSellPrices())


class TestMeanReversion:
    def test_converges_toward_mu(self):
        mu = 100.0
        regimes = [(MeanReversion(mu=mu, theta=0.5, sigma=0.1), range(0, 200))]
        md = MarketData(50.0, 49.0, regimes, seed=SEED)
        buys = md.getBuyPrices()
        assert abs(buys[-1] - mu) < abs(50.0 - mu)


class TestDrop:
    def test_average_trend_down(self):
        regimes = [(Drop(rate=0.01), range(0, NUM_DAYS))]
        md = MarketData(100.0, 99.0, regimes, seed=SEED)
        buys = md.getBuyPrices()
        assert buys[-1] < buys[0]


class TestSpike:
    def test_average_trend_up(self):
        regimes = [(Spike(rate=0.05), range(0, NUM_DAYS))]
        md = MarketData(100.0, 99.0, regimes, seed=SEED)
        buys = md.getBuyPrices()
        assert buys[-1] > buys[0]


class TestRandomWalk:
    def test_no_crash(self):
        regimes = [(RandomWalk(volatility=0.01), range(0, NUM_DAYS))]
        md = MarketData(100.0, 99.0, regimes, seed=SEED)
        assert all(p > 0 for p in md.getBuyPrices())
        assert all(p > 0 for p in md.getSellPrices())


class TestSineWave:
    def test_no_crash(self):
        regimes = [(SineWave(volatility=0.01, amplitude=1.0, phase=0.0), range(0, NUM_DAYS))]
        md = MarketData(100.0, 99.0, regimes, seed=SEED)
        assert all(p > 0 for p in md.getBuyPrices())
        assert all(p > 0 for p in md.getSellPrices())


class TestJumpDiffusion:
    def test_prices_stay_positive(self):
        regimes = [(JumpDiffusion(mu=0.0, sigma=0.02, jump_intensity=0.1, jump_size=0.05), range(0, NUM_DAYS))]
        md = MarketData(100.0, 99.0, regimes, seed=SEED)
        assert all(p > 0 for p in md.getBuyPrices())
        assert all(p > 0 for p in md.getSellPrices())


class TestMultiRegime:
    def test_regime_transitions(self):
        regimes = [
            (Drop(rate=0.05), range(0, 50)),
            (Spike(rate=0.05), range(50, 100)),
        ]
        md = MarketData(100.0, 99.0, regimes, seed=SEED)
        buys = md.getBuyPrices()
        assert buys[49] < buys[0]
        assert buys[99] > buys[50]

    def test_overlapping_regimes_later_wins(self):
        regimes = [
            (Drop(rate=0.05), range(0, 100)),
            (Spike(rate=0.05), range(0, 100)),
        ]
        md = MarketData(100.0, 99.0, regimes, seed=SEED)
        buys = md.getBuyPrices()
        assert buys[-1] > buys[0]


class TestRangeQuery:
    def test_full_range(self):
        regimes = [(GBM(), range(0, 10))]
        md = MarketData(100.0, 99.0, regimes, seed=SEED)
        buys = md.getBuyPrices()
        assert len(buys) == 11  # day 0 through day 10

    def test_slice(self):
        regimes = [(GBM(), range(0, 10))]
        md = MarketData(100.0, 99.0, regimes, seed=SEED)
        full = md.getBuyPrices()
        sliced = md.getBuyPrices(2, 5)
        assert sliced == full[2:5]

    def test_total_days(self):
        regimes = [(GBM(), range(0, 42))]
        md = MarketData(100.0, 99.0, regimes, seed=SEED)
        assert md.getTotalDays() == 42

    def test_invalid_range_raises(self):
        regimes = [(GBM(), range(0, 10))]
        md = MarketData(100.0, 99.0, regimes, seed=SEED)
        try:
            md.getBuyPrices(5, 3)  # start > end
            raised = False
        except IndexError:
            raised = True
        assert raised

    def test_buy_always_gte_sell(self):
        regimes = [(GBM(mu=0.001, sigma=0.05), range(0, NUM_DAYS))]
        md = MarketData(100.0, 99.0, regimes, seed=SEED)
        buys = md.getBuyPrices()
        sells = md.getSellPrices()
        for b, s in zip(buys, sells):
            assert b >= s


class TestDefaultParams:
    def test_gbm_defaults(self):
        md = MarketData(100.0, 99.0, [(GBM(), range(0, 10))], seed=SEED)
        assert all(p > 0 for p in md.getBuyPrices())

    def test_mean_reversion_defaults(self):
        md = MarketData(100.0, 99.0, [(MeanReversion(), range(0, 10))], seed=SEED)
        assert all(p > 0 for p in md.getBuyPrices())

    def test_jump_diffusion_defaults(self):
        md = MarketData(100.0, 99.0, [(JumpDiffusion(), range(0, 10))], seed=SEED)
        assert all(p > 0 for p in md.getBuyPrices())

    def test_drop_defaults(self):
        md = MarketData(100.0, 99.0, [(Drop(), range(0, 10))], seed=SEED)
        assert all(p > 0 for p in md.getBuyPrices())

    def test_spike_defaults(self):
        md = MarketData(100.0, 99.0, [(Spike(), range(0, 10))], seed=SEED)
        assert all(p > 0 for p in md.getBuyPrices())

    def test_random_walk_defaults(self):
        md = MarketData(100.0, 99.0, [(RandomWalk(), range(0, 10))], seed=SEED)
        assert all(p > 0 for p in md.getBuyPrices())

    def test_sine_wave_defaults(self):
        md = MarketData(100.0, 99.0, [(SineWave(), range(0, 10))], seed=SEED)
        assert all(p > 0 for p in md.getBuyPrices())
