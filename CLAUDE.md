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

- **`main.cpp`** — pybind11 module definition. Exposes all regime classes (RandomWalkRegime, SineWaveRegime, DropRegime, SpikeRegime, GBMRegime, MeanReversionRegime, JumpDiffusionRegime), RegimeAssignment struct, and _MarketData class. All regimes have default parameter values.
- **`MarketData.h/cpp`** — Core simulation engine. Constructor takes `(float startBuy, float startSell, vector<RegimeAssignment> regimes, optional<unsigned int> seed)`. Builds a per-day regime vector from assignments where later entries overwrite earlier ones. Precomputes prices using `mt19937` RNG seeded with provided seed or a random value.
- **`Regime.h`** — Abstract `Regime` base class with `update(float, mt19937&)` virtual method. Subclasses implement stochastic price movement models:
  - **RandomWalkRegime** — Simple random walk with configurable step size.
  - **SineWaveRegime** — Deterministic sine wave pattern with configurable amplitude and frequency.
  - **DropRegime** — Applies a sharp price drop with configurable magnitude.
  - **SpikeRegime** — Applies a sharp price spike with configurable magnitude.
  - **GBMRegime** — Geometric Brownian Motion with configurable drift and volatility.
  - **MeanReversionRegime** — Mean-reverting process with configurable mean, speed, and volatility.
  - **JumpDiffusionRegime** — Jump-diffusion process combining continuous drift/volatility with random jumps.
- **`Regime.cpp`** — All regime implementations.
- **`RegimeAssignment`** — Struct mapping a Regime instance to a day range `[start, end)`.


### Python Surface (`src/mm_game/`)

- **`__init__.py`** — Re-exports all regime classes, RegimeAssignment, MarketData, `__version__`, and `__doc__` from compiled `_core` extension. Provides `MarketData()` wrapper that converts `(regime, range)` tuples to RegimeAssignment objects for convenience.
- Usage: `from mm_game import MarketData, GBM; md = MarketData(100.0, 99.0, [(GBM(), range(0, 60))], seed=42); md.getNextBuyPrice()`

### Critical Behavior

`getNextBuyPrice()` and `getNextSellPrice()` must **both** be called each day to advance the internal day counter. Calling only one will not advance to the next day. The `seed` parameter enables reproducible simulations; if not provided, the engine uses a random seed.

## Code Style

- Python: Ruff enforces extensive lint rules (bugbear, isort, pylint, pytest-style, etc.) with `from __future__ import annotations` required in all files.
- C++: No `.clang-format`; no enforced C++ formatter.
- CMake: cmake-format enforced via pre-commit.

## Testing

pytest >= 8.0 with strict config: `xfail_strict = true`, warnings as errors, `--showlocals --strict-markers`. Note: the `tests/` directory is referenced in config but does not yet exist.
