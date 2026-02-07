from __future__ import annotations

from mm_game import (
    MarketData,
    DeadCatBounce,
    Drop,
    Earnings,
    GBM,
    InverseDeadCatBounce,
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


class TestEarnings:
    def test_price_reaches_target_range(self):
        target_min, target_max = 80.0, 120.0
        regimes = [(Earnings(target_min=target_min, target_max=target_max, num_days=30), range(0, 30))]
        md = MarketData(100.0, 99.0, regimes, seed=SEED)
        buys = md.getBuyPrices()
        # Final price should be near the target range (within noise tolerance)
        assert target_min * 0.9 <= buys[-1] <= target_max * 1.1

    def test_reproducibility(self):
        regimes1 = [(Earnings(target_min=80.0, target_max=120.0, num_days=20), range(0, 20))]
        regimes2 = [(Earnings(target_min=80.0, target_max=120.0, num_days=20), range(0, 20))]
        md1 = MarketData(100.0, 99.0, regimes1, seed=SEED)
        md2 = MarketData(100.0, 99.0, regimes2, seed=SEED)
        assert md1.getBuyPrices() == md2.getBuyPrices()

    def test_defaults(self):
        md = MarketData(100.0, 99.0, [(Earnings(), range(0, 10))], seed=SEED)
        assert all(p > 0 for p in md.getBuyPrices())


class TestDeadCatBounce:
    def test_three_phase_pattern(self):
        num_days = 60
        regimes = [(DeadCatBounce(drop_rate=0.3, recovery_rate=0.5, decline_rate=0.2, num_days=num_days), range(0, num_days))]
        md = MarketData(100.0, 99.0, regimes, seed=SEED)
        buys = md.getBuyPrices()
        phase1_end = num_days * 30 // 100  # day 18
        phase2_end = num_days * 60 // 100  # day 36
        # Phase 1: price should drop
        assert buys[phase1_end] < buys[0]
        # Phase 2: price should recover (higher than phase 1 bottom)
        assert buys[phase2_end] > buys[phase1_end]
        # Phase 3: price should decline again (lower than phase 2 peak)
        assert buys[-1] < buys[phase2_end]

    def test_final_price_lower_than_start(self):
        num_days = 60
        regimes = [(DeadCatBounce(drop_rate=0.3, recovery_rate=0.5, decline_rate=0.2, num_days=num_days), range(0, num_days))]
        md = MarketData(100.0, 99.0, regimes, seed=SEED)
        buys = md.getBuyPrices()
        assert buys[-1] < buys[0]

    def test_defaults(self):
        md = MarketData(100.0, 99.0, [(DeadCatBounce(), range(0, 30))], seed=SEED)
        assert all(p > 0 for p in md.getBuyPrices())


class TestInverseDeadCatBounce:
    def test_three_phase_pattern(self):
        num_days = 60
        regimes = [(InverseDeadCatBounce(rise_rate=0.3, pullback_rate=0.5, continue_rate=0.2, num_days=num_days), range(0, num_days))]
        md = MarketData(100.0, 99.0, regimes, seed=SEED)
        buys = md.getBuyPrices()
        phase1_end = num_days * 30 // 100
        phase2_end = num_days * 60 // 100
        # Phase 1: price should rise
        assert buys[phase1_end] > buys[0]
        # Phase 2: price should pull back
        assert buys[phase2_end] < buys[phase1_end]
        # Phase 3: price should continue rising
        assert buys[-1] > buys[phase2_end]

    def test_final_price_higher_than_start(self):
        num_days = 60
        regimes = [(InverseDeadCatBounce(rise_rate=0.3, pullback_rate=0.5, continue_rate=0.2, num_days=num_days), range(0, num_days))]
        md = MarketData(100.0, 99.0, regimes, seed=SEED)
        buys = md.getBuyPrices()
        assert buys[-1] > buys[0]

    def test_defaults(self):
        md = MarketData(100.0, 99.0, [(InverseDeadCatBounce(), range(0, 30))], seed=SEED)
        assert all(p > 0 for p in md.getBuyPrices())
