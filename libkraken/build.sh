#!/bin/bash

NUM_THREADS=$(grep -c ^processor /proc/cpuinfo)
echo "Using $NUM_THREADS threads for building"
HOST_ARCH=$(uname -m)
echo "Detected a $HOST_ARCH host"

# Use detected host architecture to determine compiler for cross compilation
if [ "$HOST_ARCH" == "x86_64" ]; then
	export CC=aarch64-linux-gnu-gcc-14
	export CXX=aarch64-linux-gnu-g++-14
else
	export CC=gcc-14
	export CXX=g++-14
fi

# Build debug libraries
echo "Building debug libraries.."
cmake -B build-debug -G "Unix Makefiles" \
	-DCMAKE_POSITION_INDEPENDENT_CODE=ON \
	-DCMAKE_BUILD_TYPE=Debug \
	-DCMAKE_C_FLAGS=-O0
cmake --build build-debug/ -- -j $NUM_THREADS
echo "Built debug libraries"

# Build release libraries
echo "Building release libraries.."
cmake -B build-release -G "Unix Makefiles" \
	-DCMAKE_POSITION_INDEPENDENT_CODE=ON \
	-DCMAKE_BUILD_TYPE=Release \
	-DCMAKE_C_FLAGS=-O2
cmake --build build-release/ -- -j $NUM_THREADS
echo "Built release libraries"