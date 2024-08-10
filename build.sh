#!/usr/bin/env bash

BUILD_DIRECTORY="build"
OUTPUT_DIRECTORY="out"

# Stop on error.
set -e

# Check code style.
clang-format --dry-run --Werror src/*.cpp include/*.hpp
black --check --line-length 80 python/main.py python/moire_converter.py

# Configure C++ code.
mkdir -p ${BUILD_DIRECTORY}
cmake . -B ${BUILD_DIRECTORY}

# Build C++ code into a `language` executable.
cmake --build build

# Construct TeX file, `moire_converter` will use `language` executable
# internally.
mkdir -p ${OUTPUT_DIRECTORY}
python python/moire_converter.py \
    --input data/text.moi --output ${OUTPUT_DIRECTORY}/text.tex --format tex

# Construct PDF file.
cd out
xelatex text.tex
xelatex text.tex
