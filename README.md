# mm_game

A C++/Python hybrid package that simulates market buy/sell price movements over 60 days. The core simulation engine is written in C++ and exposed to Python via [pybind11][] and [scikit-build-core][].

## Features

- Simulates 60 days of buy and sell price data from configurable starting prices
- Multiple price behavior patterns applied per day:
  - Sine wave oscillation with noise (base pattern)
  - Price drops on even days from day 10+
  - Price spikes on select days
  - Random noise every 7th day
- Sequential day-by-day price access via `getNextBuyPrice()` and `getNextSellPrice()`

## Requirements

- Python >= 3.9
- CMake >= 3.15
- A C++ compiler (e.g. Apple Clang via `xcode-select --install` on macOS)

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
from mm_game import MarketData

md = MarketData(100.0, 99.0)  # starting buy price, starting sell price

for day in range(60):
    buy = md.getNextBuyPrice()
    sell = md.getNextSellPrice()
    print(f"Day {day}: buy={buy:.2f}, sell={sell:.2f}")
```

**Note:** Both `getNextBuyPrice()` and `getNextSellPrice()` must be called each day to advance to the next day.

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
```

## License

Provided under a BSD-style license. See the [LICENSE](LICENSE) file for details.

[pybind11]: https://pybind11.readthedocs.io
[scikit-build-core]: https://scikit-build-core.readthedocs.io
