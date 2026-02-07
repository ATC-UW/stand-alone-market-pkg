from __future__ import annotations

import matplotlib.pyplot as plt
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

NUM_DAYS = 120
SEED = 42

presets = [
    ("BullQuiet", BullQuiet()),
    ("BullVolatile", BullVolatile()),
    ("BearQuiet", BearQuiet()),
    ("BearVolatile", BearVolatile()),
    ("SidewaysQuiet", SidewaysQuiet()),
    ("Crisis", Crisis()),
    ("DisbeliefMomentum", DisbeliefMomentum()),
    ("FrenzyZone", FrenzyZone()),
    ("ChopZone", ChopZone()),
    ("Transition", Transition()),
]

fig, axes = plt.subplots(5, 2, figsize=(16, 20))
axes = axes.flatten()

for idx, (name, regime) in enumerate(presets):
    md = MarketData(100.0, 99.5, [(regime, range(0, NUM_DAYS))], seed=SEED)

    buy_prices = md.getBuyPrices()
    sell_prices = md.getSellPrices()
    days = list(range(len(buy_prices)))

    ax = axes[idx]
    ax.plot(days, buy_prices, label="Buy", linewidth=1.2)
    ax.plot(days, sell_prices, label="Sell", linewidth=1.2)
    ax.set_title(name, fontsize=12, fontweight="bold")
    ax.set_xlabel("Day")
    ax.set_ylabel("Price")
    ax.legend(fontsize=8)
    ax.grid(True, alpha=0.3)

plt.suptitle("All Preset Regimes (120 days, seed=42)", fontsize=14, fontweight="bold")
plt.tight_layout()
plt.savefig("scripts/presets_plot.png", dpi=150)
print("Plot saved to scripts/presets_plot.png")
