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

build/language table \
    "labial,labial;dental,dental,alveolar,postalveolar,retroflex,palatal,velar,uvular,pharyngeal,glottal" \
    "plosive;voiceless,plosive;voiced,nasal;voiceless,nasal;voiced,trill;voiceless,trill;voiced,tap/flap;voiceless,tap/flap;voiced,sibilant_fricative;voiceless,sibilant_fricative;voiced,non-sibilant_fricative;voiceless,non-sibilant_fricative;voiced,lateral_fricative;voiceless,lateral_fricative;voiced,approximant;voiced,lateral_approximant;voiceless,lateral_approximant;voiced" \
    > out/ipa.tex
cp data/*.tex out/

# Construct PDF file.
cd out
xelatex table.tex
xelatex text.tex
