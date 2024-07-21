#!/usr/bin/env bash

BUILD_DIRECTORY="build"

set -e

rm -rf ${BUILD_DIRECTORY}
mkdir ${BUILD_DIRECTORY}
cmake . -B ${BUILD_DIRECTORY}
cd ${BUILD_DIRECTORY}
make -j4
cd ..
mkdir -p out
build/language > out/main.tex
xelatex -output-directory out out/main.tex
