#!/bin/bash
set -e

BUILD_TYPE="Debug"

if [ "$1" = "release" ]; then
    BUILD_TYPE="Release"
elif [ "$1" = "debug" ] || [ -z "$1" ]; then
    BUILD_TYPE="Debug"
else
    echo "usage: ./run.sh [debug|release]"
    exit 1
fi

cmake -S . -B build -DCMAKE_BUILD_TYPE=$BUILD_TYPE
cmake --build build --parallel $(nproc)
./build/renderer
