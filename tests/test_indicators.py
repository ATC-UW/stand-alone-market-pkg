from __future__ import annotations

import math

from mm_game import GBM, MarketData

SEED = 42
NUM_DAYS = 100


class TestSMA:
    def test_known_values(self):
        """SMA of [1,2,3,4,5] with period=3 should be [NaN, NaN, 2, 3, 4]."""
        md = MarketData(1.0, 0.5, [(GBM(mu=0.0, sigma=0.0), range(0, 4))], seed=SEED)
        # GBM with sigma=0 and mu=0 gives constant prices, so use a real sim
        # Instead test with actual sim data: SMA should equal manual calculation
        md = MarketData(100.0, 99.0, [(GBM(), range(0, NUM_DAYS))], seed=SEED)
        prices = md.getBuyPrices()
        sma = md.getBuySMA(period=3)
        assert len(sma) == len(prices)
        # First 2 values should be NaN
        assert math.isnan(sma[0])
        assert math.isnan(sma[1])
        # Third value should be average of first 3 prices
        expected = sum(prices[:3]) / 3
        assert abs(sma[2] - expected) < 1e-4

    def test_nan_before_period(self):
        md = MarketData(100.0, 99.0, [(GBM(), range(0, NUM_DAYS))], seed=SEED)
        sma = md.getBuySMA(period=20)
        for i in range(19):
            assert math.isnan(sma[i])
        assert not math.isnan(sma[19])

    def test_sell_sma(self):
        md = MarketData(100.0, 99.0, [(GBM(), range(0, NUM_DAYS))], seed=SEED)
        sell_sma = md.getSellSMA(period=10)
        sell_prices = md.getSellPrices()
        expected = sum(sell_prices[:10]) / 10
        assert abs(sell_sma[9] - expected) < 1e-4

    def test_different_periods_cached_separately(self):
        md = MarketData(100.0, 99.0, [(GBM(), range(0, NUM_DAYS))], seed=SEED)
        sma5 = md.getBuySMA(period=5)
        sma20 = md.getBuySMA(period=20)
        # They should differ (different periods)
        assert sma5[25] != sma20[25]

    def test_range_slicing(self):
        md = MarketData(100.0, 99.0, [(GBM(), range(0, NUM_DAYS))], seed=SEED)
        full = md.getBuySMA(period=5)
        sliced = md.getBuySMA(period=5, start=10, end=20)
        assert sliced == full[10:20]


class TestEMA:
    def test_nan_before_period(self):
        md = MarketData(100.0, 99.0, [(GBM(), range(0, NUM_DAYS))], seed=SEED)
        ema = md.getBuyEMA(period=20)
        for i in range(19):
            assert math.isnan(ema[i])
        assert not math.isnan(ema[19])

    def test_first_ema_equals_sma(self):
        """First EMA value should equal the SMA (used as seed)."""
        md = MarketData(100.0, 99.0, [(GBM(), range(0, NUM_DAYS))], seed=SEED)
        ema = md.getBuyEMA(period=10)
        sma = md.getBuySMA(period=10)
        assert abs(ema[9] - sma[9]) < 1e-4

    def test_sell_ema(self):
        md = MarketData(100.0, 99.0, [(GBM(), range(0, NUM_DAYS))], seed=SEED)
        sell_ema = md.getSellEMA(period=10)
        assert not math.isnan(sell_ema[9])

    def test_range_slicing(self):
        md = MarketData(100.0, 99.0, [(GBM(), range(0, NUM_DAYS))], seed=SEED)
        full = md.getBuyEMA(period=5)
        sliced = md.getBuyEMA(period=5, start=10, end=20)
        assert sliced == full[10:20]


class TestRSI:
    def test_nan_before_period(self):
        md = MarketData(100.0, 99.0, [(GBM(), range(0, NUM_DAYS))], seed=SEED)
        rsi = md.getBuyRSI(period=14)
        for i in range(14):
            assert math.isnan(rsi[i])
        assert not math.isnan(rsi[14])

    def test_rsi_bounded_0_100(self):
        md = MarketData(100.0, 99.0, [(GBM(), range(0, NUM_DAYS))], seed=SEED)
        rsi = md.getBuyRSI(period=14)
        for val in rsi:
            if not math.isnan(val):
                assert 0.0 <= val <= 100.0

    def test_sell_rsi(self):
        md = MarketData(100.0, 99.0, [(GBM(), range(0, NUM_DAYS))], seed=SEED)
        sell_rsi = md.getSellRSI(period=14)
        for val in sell_rsi:
            if not math.isnan(val):
                assert 0.0 <= val <= 100.0

    def test_range_slicing(self):
        md = MarketData(100.0, 99.0, [(GBM(), range(0, NUM_DAYS))], seed=SEED)
        full = md.getBuyRSI(period=14)
        sliced = md.getBuyRSI(period=14, start=20, end=40)
        assert sliced == full[20:40]

    def test_reproducibility(self):
        md1 = MarketData(100.0, 99.0, [(GBM(), range(0, NUM_DAYS))], seed=SEED)
        md2 = MarketData(100.0, 99.0, [(GBM(), range(0, NUM_DAYS))], seed=SEED)
        assert md1.getBuyRSI() == md2.getBuyRSI()


class TestMACD:
    def test_returns_three_lists(self):
        md = MarketData(100.0, 99.0, [(GBM(), range(0, NUM_DAYS))], seed=SEED)
        macd_line, signal_line, histogram = md.getBuyMACD()
        assert len(macd_line) == len(md.getBuyPrices())
        assert len(signal_line) == len(md.getBuyPrices())
        assert len(histogram) == len(md.getBuyPrices())

    def test_histogram_equals_macd_minus_signal(self):
        md = MarketData(100.0, 99.0, [(GBM(), range(0, NUM_DAYS))], seed=SEED)
        macd_line, signal_line, histogram = md.getBuyMACD()
        for m, s, h in zip(macd_line, signal_line, histogram):
            if not (math.isnan(m) or math.isnan(s) or math.isnan(h)):
                assert abs(h - (m - s)) < 1e-4

    def test_sell_macd(self):
        md = MarketData(100.0, 99.0, [(GBM(), range(0, NUM_DAYS))], seed=SEED)
        macd_line, signal_line, histogram = md.getSellMACD()
        assert len(macd_line) == len(md.getSellPrices())

    def test_range_slicing(self):
        md = MarketData(100.0, 99.0, [(GBM(), range(0, NUM_DAYS))], seed=SEED)
        full_m, full_s, full_h = md.getBuyMACD()
        sl_m, sl_s, sl_h = md.getBuyMACD(start=30, end=50)
        assert sl_m == full_m[30:50]
        assert sl_s == full_s[30:50]
        assert sl_h == full_h[30:50]


class TestBollingerBands:
    def test_returns_three_lists(self):
        md = MarketData(100.0, 99.0, [(GBM(), range(0, NUM_DAYS))], seed=SEED)
        upper, middle, lower = md.getBuyBollingerBands()
        assert len(upper) == len(md.getBuyPrices())
        assert len(middle) == len(md.getBuyPrices())
        assert len(lower) == len(md.getBuyPrices())

    def test_upper_gte_middle_gte_lower(self):
        md = MarketData(100.0, 99.0, [(GBM(), range(0, NUM_DAYS))], seed=SEED)
        upper, middle, lower = md.getBuyBollingerBands()
        for u, m, l in zip(upper, middle, lower):
            if not (math.isnan(u) or math.isnan(m) or math.isnan(l)):
                assert u >= m >= l

    def test_middle_equals_sma(self):
        md = MarketData(100.0, 99.0, [(GBM(), range(0, NUM_DAYS))], seed=SEED)
        _, middle, _ = md.getBuyBollingerBands(period=20)
        sma = md.getBuySMA(period=20)
        for m, s in zip(middle, sma):
            if not math.isnan(m):
                assert abs(m - s) < 1e-4

    def test_sell_bollinger(self):
        md = MarketData(100.0, 99.0, [(GBM(), range(0, NUM_DAYS))], seed=SEED)
        upper, middle, lower = md.getSellBollingerBands()
        assert len(upper) == len(md.getSellPrices())

    def test_range_slicing(self):
        md = MarketData(100.0, 99.0, [(GBM(), range(0, NUM_DAYS))], seed=SEED)
        full_u, full_m, full_l = md.getBuyBollingerBands()
        sl_u, sl_m, sl_l = md.getBuyBollingerBands(start=25, end=50)
        assert sl_u == full_u[25:50]
        assert sl_m == full_m[25:50]
        assert sl_l == full_l[25:50]


class TestATR:
    def test_nan_before_period(self):
        md = MarketData(100.0, 99.0, [(GBM(), range(0, NUM_DAYS))], seed=SEED)
        atr = md.getATR(period=14)
        for i in range(13):
            assert math.isnan(atr[i])
        assert not math.isnan(atr[13])

    def test_atr_positive(self):
        md = MarketData(100.0, 99.0, [(GBM(), range(0, NUM_DAYS))], seed=SEED)
        atr = md.getATR(period=14)
        for val in atr:
            if not math.isnan(val):
                assert val >= 0.0

    def test_range_slicing(self):
        md = MarketData(100.0, 99.0, [(GBM(), range(0, NUM_DAYS))], seed=SEED)
        full = md.getATR(period=14)
        sliced = md.getATR(period=14, start=20, end=40)
        assert sliced == full[20:40]

    def test_reproducibility(self):
        md1 = MarketData(100.0, 99.0, [(GBM(), range(0, NUM_DAYS))], seed=SEED)
        md2 = MarketData(100.0, 99.0, [(GBM(), range(0, NUM_DAYS))], seed=SEED)
        assert md1.getATR() == md2.getATR()


class TestIndicatorEdgeCases:
    def test_period_greater_than_days(self):
        """All values should be NaN when period exceeds total days."""
        md = MarketData(100.0, 99.0, [(GBM(), range(0, 5))], seed=SEED)
        sma = md.getBuySMA(period=20)
        assert all(math.isnan(v) for v in sma)

    def test_period_one(self):
        """SMA with period=1 should equal the prices themselves."""
        md = MarketData(100.0, 99.0, [(GBM(), range(0, NUM_DAYS))], seed=SEED)
        sma = md.getBuySMA(period=1)
        prices = md.getBuyPrices()
        for s, p in zip(sma, prices):
            assert abs(s - p) < 1e-4

    def test_caching_returns_same_values(self):
        """Calling the same indicator twice returns identical results."""
        md = MarketData(100.0, 99.0, [(GBM(), range(0, NUM_DAYS))], seed=SEED)
        rsi1 = md.getBuyRSI(period=14)
        rsi2 = md.getBuyRSI(period=14)
        assert rsi1 == rsi2
