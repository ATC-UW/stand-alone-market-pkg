from __future__ import annotations

import matplotlib.pyplot as plt
from mm_game import (
    MarketData,
    Drop,
    GBM,
    JumpDiffusion,
    MeanReversion,
    Spike,
)

regimes = [
    ("MeanReversion", MeanReversion(mu=100.0, theta=0.1, sigma=0.5), range(0, 30)),
    ("GBM", GBM(mu=0.0005, sigma=0.02), range(30, 60)),
    ("Drop", Drop(rate=0.01), range(60, 80)),
    ("JumpDiffusion", JumpDiffusion(mu=0.0, sigma=0.02, jump_intensity=0.1, jump_size=0.05), range(80, 100)),
    ("Spike", Spike(rate=0.03), range(100, 120)),
]

total_days = max(r.stop for _, _, r in regimes)
md = MarketData(
    start_buy_price=100.0,
    start_sell_price=99.5,
    regimes=[(regime, days) for _, regime, days in regimes],
    seed=42,
)

buy_prices = []
sell_prices = []
for _ in range(total_days + 1):
    buy_prices.append(md.getNextBuyPrice())
    sell_prices.append(md.getNextSellPrice())

days = list(range(total_days + 1))

plt.figure(figsize=(14, 6))
plt.plot(days, buy_prices, label="Buy Price", linewidth=1.5)
plt.plot(days, sell_prices, label="Sell Price", linewidth=1.5)

# Draw regime boundaries
colors = ["#e6f3ff", "#fff3e6", "#ffe6e6", "#e6ffe6", "#f3e6ff"]
for i, (name, _, day_range) in enumerate(regimes):
    plt.axvspan(day_range.start, day_range.stop, alpha=0.2, color=colors[i % len(colors)])
    mid = (day_range.start + day_range.stop) / 2
    plt.text(mid, plt.ylim()[1], name, ha="center", va="top", fontsize=8, style="italic")

plt.xlabel("Day")
plt.ylabel("Price")
plt.title("Multi-Regime Price Simulation")
plt.legend()
plt.grid(True, alpha=0.3)
plt.tight_layout()
plt.savefig("scripts/price_plot.png", dpi=150)
print("Plot saved to scripts/price_plot.png")
