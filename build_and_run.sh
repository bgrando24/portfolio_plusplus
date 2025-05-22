#!/usr/bin/env bash
set -euo pipefail

# (Optional) enable ccache for faster repeated builds
export CC="ccache clang"
export CXX="ccache clang++"

# 1) Configure once (only if the build folder is missing or stale)
if [ ! -f build/CMakeCache.txt ]; then
  cmake -S . -B build \
    -DCMAKE_BUILD_TYPE=Debug \
    -G Ninja
fi

# 2) build only what’s changed
cmake --build build -- -j

# 3) run the binary
./build/portfolio_plusplus

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# # clean up old build and any caching
# rm -rf build
# rm -rf python/src/__pycache__

# # cmake build
# cmake -S . -B build
# cmake --build build

# # run executable
# ./build/portfolio_plusplus