# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

`mm_game` is a C++/Python hybrid package that exposes a market price simulation engine via pybind11. It simulates buy/sell price movements across a configurable number of days using regime-based price modifiers with support for reproducible simulations via seeding. Distributed as a Python wheel on PyPI.

## Build & Development Commands

```bash
# Install the package (builds C++ extension)
pip install .

# Editable install with compile commands (for development)
pip install -e . -Ccmake.define.CMAKE_EXPORT_COMPILE_COMMANDS=1 -Cbuild-dir=build

# Build sdist and wheel
python -m build

# Upload to PyPI
python -m twine upload dist/*

# Run linting (pre-commit hooks: ruff, cmake-format)
nox -s lint

# Run tests
nox -s tests
# or directly:
pytest

# Set up dev environment (.venv with editable install)
nox -s dev

# Run a single test
pytest tests/test_file.py::test_name
```

## Build Toolchain

scikit-build-core + CMake (3.15–3.27) + pybind11. The CMake build compiles `src/main.cpp` into a `_core` Python extension module installed into the `mm_game` package.

## Architecture

### C++ Core (`src/`)

- **`main.cpp`** — pybind11 module definition. Exposes all regime classes (RandomWalk, SineWave, Drop, Spike, GBM, MeanReversion, JumpDiffusion, Momentum, TrendingMeanReversion), RegimeAssignment struct, and _MarketData class. All regimes have default parameter values.
- **`MarketData.h/cpp`** — Core simulation engine. Constructor takes `(float startBuy, float startSell, vector<RegimeAssignment> regimes, optional<unsigned int> seed)`. Builds a per-day regime vector from assignments where later entries overwrite earlier ones. Precomputes all prices at construction time using `mt19937` RNG. Provides `getBuyPrices(start, end)`, `getSellPrices(start, end)`, and `getTotalDays()` range query methods.
- **`Regime.h`** — Abstract `Regime` base class with `update(float, mt19937&)` virtual method. Subclasses implement stochastic price movement models:
  - **RandomWalkRegime** — Simple random walk with configurable step size.
  - **SineWaveRegime** — Deterministic sine wave pattern with configurable amplitude and phase. Uses `setDayIndex()` override.
  - **DropRegime** — Applies a multiplicative price drop.
  - **SpikeRegime** — Applies a multiplicative price spike.
  - **GBMRegime** — Geometric Brownian Motion with configurable drift and volatility.
  - **MeanReversionRegime** — Ornstein-Uhlenbeck mean-reverting process.
  - **JumpDiffusionRegime** — Jump-diffusion combining GBM with Poisson-driven jumps.
  - **MomentumRegime** — GBM with autocorrelated returns (momentum factor).
  - **TrendingMeanReversionRegime** — Mean reversion with linearly drifting mean.
- **`Regime.cpp`** — All regime implementations.
- **`RegimeAssignment`** — Struct mapping a Regime instance to a day range `[start, end)`.

### Python Surface (`src/mm_game/`)

- **`__init__.py`** — Re-exports all regime classes, RegimeAssignment, MarketData, `__version__`, and `__doc__` from compiled `_core` extension. Provides `MarketData()` wrapper that converts `(regime, range)` tuples to RegimeAssignment objects.
- **`presets.py`** — Factory functions for common market conditions: BullQuiet, BullVolatile, BearQuiet, BearVolatile, SidewaysQuiet, Crisis, DisbeliefMomentum, FrenzyZone, ChopZone, Transition. Each accepts `scale` (and optionally `mu`) parameters.
- Usage: `from mm_game import MarketData, GBM; md = MarketData(100.0, 99.0, [(GBM(), range(0, 60))], seed=42); prices = md.getBuyPrices()`

### API

All prices are precomputed at construction time. Query with:
- `getBuyPrices(start=0, end=-1)` — Returns list of buy prices. Default returns all.
- `getSellPrices(start=0, end=-1)` — Returns list of sell prices. Default returns all.
- `getTotalDays()` — Returns the number of simulation days.

The `seed` parameter enables reproducible simulations; if not provided, the engine uses a time-based seed.

## Code Style

- Python: Ruff enforces extensive lint rules (bugbear, isort, pylint, pytest-style, etc.) with `from __future__ import annotations` required in all files.
- C++: No `.clang-format`; no enforced C++ formatter.
- CMake: cmake-format enforced via pre-commit.

## Testing

pytest >= 8.0 with strict config: `xfail_strict = true`, warnings as errors, `--showlocals --strict-markers`. Tests are in `tests/test_regimes.py` and `tests/test_presets.py`.
