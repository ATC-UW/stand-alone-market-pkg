"""Plot all technical indicators for a simulated market using mid prices."""
from __future__ import annotations

import math

import matplotlib.pyplot as plt

from mm_game import GBM, Drop, MarketData, Spike

# Create a multi-regime simulation
NUM_DAYS = 200
md = MarketData(
    100.0,
    99.0,
    [
        (GBM(mu=0.001, sigma=0.02), range(0, 80)),
        (Drop(rate=0.02), range(80, 120)),
        (Spike(rate=0.015), range(120, 160)),
        (GBM(mu=0.0, sigma=0.03), range(160, NUM_DAYS)),
    ],
    seed=42,
)

days = list(range(len(md.getMidPrices())))
mid = md.getMidPrices()
buy = md.getBuyPrices()
sell = md.getSellPrices()

# Fetch indicators on mid prices
sma_20 = md.getMidSMA(period=20)
ema_20 = md.getMidEMA(period=20)
rsi_14 = md.getMidRSI(period=14)
macd_line, signal_line, histogram = md.getMidMACD()
bb_upper, bb_middle, bb_lower = md.getMidBollingerBands(period=20)
atr_14 = md.getATR(period=14)

fig, axes = plt.subplots(5, 1, figsize=(14, 16), sharex=True)
fig.suptitle("Technical Indicators Dashboard (Mid Price)", fontsize=14, fontweight="bold")

# --- 1. Price + SMA/EMA ---
ax = axes[0]
ax.plot(days, mid, label="Mid Price", color="black", linewidth=1)
ax.fill_between(days, sell, buy, alpha=0.15, color="steelblue", label="Bid-Ask Spread")
ax.plot(days, sma_20, label="SMA(20)", color="orange", linewidth=1.2)
ax.plot(days, ema_20, label="EMA(20)", color="green", linewidth=1.2)
ax.set_ylabel("Price")
ax.legend(loc="upper left", fontsize=8)
ax.set_title("Mid Price with Moving Averages")
ax.grid(alpha=0.3)

# --- 2. Bollinger Bands ---
ax = axes[1]
ax.plot(days, mid, label="Mid Price", color="black", linewidth=1)
ax.plot(days, bb_upper, label="Upper Band", color="red", linewidth=0.8, linestyle="--")
ax.plot(days, bb_middle, label="Middle (SMA 20)", color="orange", linewidth=0.8)
ax.plot(days, bb_lower, label="Lower Band", color="green", linewidth=0.8, linestyle="--")
ax.fill_between(days, bb_lower, bb_upper, alpha=0.1, color="gray")
ax.set_ylabel("Price")
ax.legend(loc="upper left", fontsize=8)
ax.set_title("Bollinger Bands (20, 2.0)")
ax.grid(alpha=0.3)

# --- 3. RSI ---
ax = axes[2]
ax.plot(days, rsi_14, label="RSI(14)", color="purple", linewidth=1)
ax.axhline(y=70, color="red", linestyle="--", linewidth=0.8, label="Overbought (70)")
ax.axhline(y=30, color="green", linestyle="--", linewidth=0.8, label="Oversold (30)")
ax.fill_between(days, 30, 70, alpha=0.05, color="gray")
ax.set_ylabel("RSI")
ax.set_ylim(0, 100)
ax.legend(loc="upper left", fontsize=8)
ax.set_title("Relative Strength Index")
ax.grid(alpha=0.3)

# --- 4. MACD ---
ax = axes[3]
ax.plot(days, macd_line, label="MACD Line", color="blue", linewidth=1)
ax.plot(days, signal_line, label="Signal Line", color="red", linewidth=1)
# Filter NaN for histogram bar colors
colors = ["green" if not math.isnan(h) and h >= 0 else "red" for h in histogram]
ax.bar(days, [0.0 if math.isnan(h) else h for h in histogram],
       color=colors, alpha=0.4, width=1.0, label="Histogram")
ax.axhline(y=0, color="black", linewidth=0.5)
ax.set_ylabel("MACD")
ax.legend(loc="upper left", fontsize=8)
ax.set_title("MACD (12, 26, 9)")
ax.grid(alpha=0.3)

# --- 5. ATR ---
ax = axes[4]
ax.plot(days, atr_14, label="ATR(14)", color="brown", linewidth=1)
ax.set_ylabel("ATR")
ax.set_xlabel("Day")
ax.legend(loc="upper left", fontsize=8)
ax.set_title("Average True Range")
ax.grid(alpha=0.3)

plt.tight_layout()
plt.savefig("indicators_plot.png", dpi=150, bbox_inches="tight")
plt.show()
print("Saved to indicators_plot.png")
