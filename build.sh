#!/bin/bash
set -e

# Create and activate virtual environment
python3 -m venv .venv
source .venv/bin/activate

# Install build dependencies
pip install --upgrade pip build twine

# Clean previous builds
rm -rf dist/

# Build sdist and wheel
python -m build

# Upload to PyPI
python -m twine upload dist/*
