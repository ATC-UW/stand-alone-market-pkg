# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

`mm_game` is a C++/Python hybrid package that exposes a market price simulation engine via pybind11. It simulates 60 days of buy/sell price movements using a factory/strategy pattern of challenge-based price modifiers. Distributed as a Python wheel on PyPI.

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

- **`main.cpp`** — pybind11 module definition. Exposes `MarketData` class with `(float, float)` constructor and `getNextBuyPrice()` / `getNextSellPrice()` methods.
- **`MarketData.h`** — Core simulation engine. Creates a pipeline of 60 `Challenge` objects that sequentially modify prices per day. Challenge assignment: base is Challenge3 (sine wave + noise), overridden by Challenge4 (drop) on even days 10+, Challenge5 (spike) on specific days, Challenge2 (random noise) every 7th day.
- **`Challenge.h`** — Abstract `Challenge` base class with `update(float)` pure virtual. Subclasses: Challenge0 (identity), Challenge1 (volatility), Challenge2 (random noise), Challenge3 (sine + noise), Challenge4 (drop), Challenge5 (spike). `ChallengeFactory` uses registry pattern (`unordered_map<int, ChallengeCreator>`) for construction by type ID.
- **`utils.h`** — `utils::random(float a, float b)` for uniform random floats.

### Python Surface (`src/mm_game/`)

- **`__init__.py`** — Re-exports `MarketData`, `__version__`, `__doc__` from compiled `_core` extension.
- Usage: `from mm_game import MarketData; md = MarketData(100.0, 99.0); md.getNextBuyPrice()`

### Critical Behavior

`getNextBuyPrice()` and `getNextSellPrice()` must **both** be called each day to advance the internal day counter. Calling only one will not advance to the next day.

## Code Style

- Python: Ruff enforces extensive lint rules (bugbear, isort, pylint, pytest-style, etc.) with `from __future__ import annotations` required in all files.
- C++: No `.clang-format`; no enforced C++ formatter.
- CMake: cmake-format enforced via pre-commit.

## Testing

pytest >= 8.0 with strict config: `xfail_strict = true`, warnings as errors, `--showlocals --strict-markers`. Note: the `tests/` directory is referenced in config but does not yet exist.
