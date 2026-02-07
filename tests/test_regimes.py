from __future__ import annotations

import pytest

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

        for _ in range(NUM_DAYS):
            assert md1.getNextBuyPrice() == md2.getNextBuyPrice()
            assert md1.getNextSellPrice() == md2.getNextSellPrice()

    def test_different_seed_different_prices(self):
        regimes = [(GBM(mu=0.0005, sigma=0.02), range(0, NUM_DAYS))]
        md1 = MarketData(100.0, 99.0, regimes, seed=1)
        md2 = MarketData(100.0, 99.0, regimes, seed=2)

        any_different = False
        for _ in range(NUM_DAYS):
            b1 = md1.getNextBuyPrice()
            b2 = md2.getNextBuyPrice()
            md1.getNextSellPrice()
            md2.getNextSellPrice()
            if b1 != b2:
                any_different = True
        assert any_different


class TestGBM:
    def test_prices_stay_positive(self):
        regimes = [(GBM(mu=0.0, sigma=0.02), range(0, NUM_DAYS))]
        md = MarketData(100.0, 99.0, regimes, seed=SEED)
        for _ in range(NUM_DAYS):
            assert md.getNextBuyPrice() > 0
            assert md.getNextSellPrice() > 0


class TestMeanReversion:
    def test_converges_toward_mu(self):
        mu = 100.0
        regimes = [(MeanReversion(mu=mu, theta=0.5, sigma=0.1), range(0, 200))]
        md = MarketData(50.0, 49.0, regimes, seed=SEED)
        last_buy = 0.0
        for _ in range(200):
            last_buy = md.getNextBuyPrice()
            md.getNextSellPrice()
        assert abs(last_buy - mu) < abs(50.0 - mu)


class TestDrop:
    def test_average_trend_down(self):
        regimes = [(Drop(rate=0.01), range(0, NUM_DAYS))]
        md = MarketData(100.0, 99.0, regimes, seed=SEED)
        first_buy = md.getNextBuyPrice()
        md.getNextSellPrice()
        last_buy = first_buy
        for _ in range(1, NUM_DAYS):
            last_buy = md.getNextBuyPrice()
            md.getNextSellPrice()
        assert last_buy < first_buy


class TestSpike:
    def test_average_trend_up(self):
        regimes = [(Spike(rate=0.05), range(0, NUM_DAYS))]
        md = MarketData(100.0, 99.0, regimes, seed=SEED)
        first_buy = md.getNextBuyPrice()
        md.getNextSellPrice()
        last_buy = first_buy
        for _ in range(1, NUM_DAYS):
            last_buy = md.getNextBuyPrice()
            md.getNextSellPrice()
        assert last_buy > first_buy


class TestRandomWalk:
    def test_no_crash(self):
        regimes = [(RandomWalk(volatility=0.01), range(0, NUM_DAYS))]
        md = MarketData(100.0, 99.0, regimes, seed=SEED)
        for _ in range(NUM_DAYS):
            buy = md.getNextBuyPrice()
            sell = md.getNextSellPrice()
            assert buy > 0
            assert sell > 0


class TestSineWave:
    def test_no_crash(self):
        regimes = [(SineWave(volatility=0.01, amplitude=1.0, phase=0.0), range(0, NUM_DAYS))]
        md = MarketData(100.0, 99.0, regimes, seed=SEED)
        for _ in range(NUM_DAYS):
            buy = md.getNextBuyPrice()
            sell = md.getNextSellPrice()
            assert buy > 0
            assert sell > 0


class TestJumpDiffusion:
    def test_prices_stay_positive(self):
        regimes = [(JumpDiffusion(mu=0.0, sigma=0.02, jump_intensity=0.1, jump_size=0.05), range(0, NUM_DAYS))]
        md = MarketData(100.0, 99.0, regimes, seed=SEED)
        for _ in range(NUM_DAYS):
            assert md.getNextBuyPrice() > 0
            assert md.getNextSellPrice() > 0


class TestMultiRegime:
    def test_regime_transitions(self):
        regimes = [
            (Drop(rate=0.05), range(0, 50)),
            (Spike(rate=0.05), range(50, 100)),
        ]
        md = MarketData(100.0, 99.0, regimes, seed=SEED)
        buys = []
        for _ in range(100):
            buys.append(md.getNextBuyPrice())
            md.getNextSellPrice()
        assert buys[49] < buys[0]
        assert buys[99] > buys[50]

    def test_overlapping_regimes_later_wins(self):
        regimes = [
            (Drop(rate=0.05), range(0, 100)),
            (Spike(rate=0.05), range(0, 100)),
        ]
        md = MarketData(100.0, 99.0, regimes, seed=SEED)
        first_buy = md.getNextBuyPrice()
        md.getNextSellPrice()
        last_buy = first_buy
        for _ in range(1, 100):
            last_buy = md.getNextBuyPrice()
            md.getNextSellPrice()
        assert last_buy > first_buy

    def test_both_methods_required_to_advance(self):
        regimes = [(GBM(), range(0, 10))]
        md = MarketData(100.0, 99.0, regimes, seed=SEED)
        price1 = md.getNextBuyPrice()
        price2 = md.getNextBuyPrice()
        assert price1 == price2


class TestDefaultParams:
    def test_gbm_defaults(self):
        regimes = [(GBM(), range(0, 10))]
        md = MarketData(100.0, 99.0, regimes, seed=SEED)
        assert md.getNextBuyPrice() > 0
        md.getNextSellPrice()

    def test_mean_reversion_defaults(self):
        regimes = [(MeanReversion(), range(0, 10))]
        md = MarketData(100.0, 99.0, regimes, seed=SEED)
        assert md.getNextBuyPrice() > 0
        md.getNextSellPrice()

    def test_jump_diffusion_defaults(self):
        regimes = [(JumpDiffusion(), range(0, 10))]
        md = MarketData(100.0, 99.0, regimes, seed=SEED)
        assert md.getNextBuyPrice() > 0
        md.getNextSellPrice()

    def test_drop_defaults(self):
        regimes = [(Drop(), range(0, 10))]
        md = MarketData(100.0, 99.0, regimes, seed=SEED)
        assert md.getNextBuyPrice() > 0
        md.getNextSellPrice()

    def test_spike_defaults(self):
        regimes = [(Spike(), range(0, 10))]
        md = MarketData(100.0, 99.0, regimes, seed=SEED)
        assert md.getNextBuyPrice() > 0
        md.getNextSellPrice()

    def test_random_walk_defaults(self):
        regimes = [(RandomWalk(), range(0, 10))]
        md = MarketData(100.0, 99.0, regimes, seed=SEED)
        assert md.getNextBuyPrice() > 0
        md.getNextSellPrice()

    def test_sine_wave_defaults(self):
        regimes = [(SineWave(), range(0, 10))]
        md = MarketData(100.0, 99.0, regimes, seed=SEED)
        assert md.getNextBuyPrice() > 0
        md.getNextSellPrice()
