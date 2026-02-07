# mm_game

A C++/Python hybrid package for simulating market buy/sell price movements with configurable regime-based dynamics. The core simulation engine is written in C++ and exposed to Python via [pybind11][] and [scikit-build-core][].

## Features

- Configurable price regimes that control market dynamics over user-defined day ranges
- Built-in regime types:
  - **GBM** — Geometric Brownian Motion (log-normal price model)
  - **MeanReversion** — Ornstein-Uhlenbeck mean-reverting process
  - **JumpDiffusion** — GBM with Poisson-driven jumps
  - **Momentum** — GBM with autocorrelated returns
  - **TrendingMeanReversion** — Mean reversion with linearly drifting mean
  - **RandomWalk** — Random directional steps
  - **SineWave** — Sine oscillation with noise
  - **Drop** — Multiplicative price drop
  - **Spike** — Multiplicative price spike
- **Preset regimes** for common market conditions: BullQuiet, BullVolatile, BearQuiet, BearVolatile, SidewaysQuiet, Crisis, DisbeliefMomentum, FrenzyZone, ChopZone, Transition
- Reproducible simulations via optional seed parameter
- All regimes have sensible default parameters
- Configurable number of simulation days (inferred from regime assignments)
- Range query API — retrieve all prices at once or slice by day range

## Requirements

- Python >= 3.9
- CMake >= 3.15
- A C++ compiler with C++17 support (e.g. Apple Clang via `xcode-select --install` on macOS)

## Installation

```bash
pip install mm_game
```

Or install from source:

```bash
git clone <repo-url>
cd stand-alone-market-pkg
pip install .
```

## Usage

```python
from mm_game import MarketData, GBM, MeanReversion, JumpDiffusion, Drop, Spike

# Define regimes as (regime, day_range) tuples
regimes = [
    (MeanReversion(mu=100.0, theta=0.1, sigma=0.5), range(0, 30)),
    (GBM(mu=0.0005, sigma=0.02), range(30, 60)),
    (Drop(rate=0.01), range(60, 80)),
    (JumpDiffusion(mu=0.0, sigma=0.02, jump_intensity=0.1, jump_size=0.05), range(80, 100)),
    (Spike(rate=0.03), range(100, 120)),
]

md = MarketData(
    start_buy_price=100.0,
    start_sell_price=99.5,
    regimes=regimes,
    seed=42,  # optional, for reproducibility
)

# Get all prices at once
buy_prices = md.getBuyPrices()        # list of floats, day 0 through last day
sell_prices = md.getSellPrices()

# Or slice by day range
buy_slice = md.getBuyPrices(30, 60)   # days 30-59 only

# Query total simulation days
total = md.getTotalDays()             # 120
```

### Preset Regimes

Presets provide tuned regime configurations for common market conditions:

```python
from mm_game import MarketData, BullQuiet, Crisis, FrenzyZone

regimes = [
    (BullQuiet(), range(0, 50)),
    (Crisis(), range(50, 80)),
    (FrenzyZone(), range(80, 120)),
]
md = MarketData(100.0, 99.5, regimes, seed=42)

buy_prices = md.getBuyPrices()
```

Available presets (all accept `scale` to adjust volatility):
- `BullQuiet(scale)` — Low-volatility upward drift
- `BullVolatile(scale)` — High-volatility upward drift
- `BearQuiet(scale)` — Low-volatility downward drift
- `BearVolatile(scale)` — High-volatility downward drift
- `SidewaysQuiet(mu, scale)` — Quiet mean reversion around `mu`
- `Crisis(scale)` — Crash dynamics with negative jumps
- `DisbeliefMomentum(mu, scale)` — Trending mean reversion with drift
- `FrenzyZone(scale)` — Momentum-driven rally
- `ChopZone(mu, scale)` — Volatile mean reversion (choppy market)
- `Transition(scale)` — Flat drift, moderate noise

### Regime Parameters

All regimes can be constructed with no arguments (using defaults):

```python
GBM()                # mu=0.0005, sigma=0.02
MeanReversion()      # mu=100.0, theta=0.1, sigma=0.5
JumpDiffusion()      # mu=0.0, sigma=0.02, jump_intensity=0.1, jump_size=0.05
Momentum()           # mu=0.0, sigma=0.02, momentum=0.0
TrendingMeanReversion()  # mu=100.0, drift=0.0, theta=0.1, sigma=0.5
RandomWalk()         # volatility=0.01
SineWave()           # volatility=0.01, amplitude=1.0, phase=0.0
Drop()               # rate=0.01
Spike()              # rate=0.05
```

### Overlapping Regimes

When day ranges overlap, later entries in the list take priority:

```python
regimes = [
    (GBM(), range(0, 100)),         # base regime
    (Drop(rate=0.05), range(50, 70)),  # overrides GBM on days 50-69
]
```

## Development

```bash
# Create a virtual environment
python3 -m venv .venv
source .venv/bin/activate

# Editable install
pip install -e . -Ccmake.define.CMAKE_EXPORT_COMPILE_COMMANDS=1 -Cbuild-dir=build

# Run linting
nox -s lint

# Run tests
nox -s tests
# or directly:
pytest
```

## License

Provided under a BSD-style license. See the [LICENSE](LICENSE) file for details.

[pybind11]: https://pybind11.readthedocs.io
[scikit-build-core]: https://scikit-build-core.readthedocs.io
