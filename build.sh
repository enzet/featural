#!/usr/bin/env bash

BUILD_DIRECTORY="build"

# Stop on error.
set -e

# Check code style.
clang-format --dry-run --Werror src/main.cpp
black --check --line-length 80 python/main.py

# Configure C++ code.
rm -rf ${BUILD_DIRECTORY}
mkdir ${BUILD_DIRECTORY}
cmake . -B ${BUILD_DIRECTORY}

# Build C++ code into a `language` executable.
cmake --build build

# Construct TeX file.
mkdir -p out
build/language > out/ipa.tex
cp data/*.tex out/

# Construct PDF file.
xelatex -output-directory out out/table.tex
xelatex -output-directory out out/text.tex
