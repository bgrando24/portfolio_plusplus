#!/usr/bin/env bash
set -euo pipefail

# ─── Determine parallelism ─────────────────────────────────────────────────────
if command -v nproc &> /dev/null; then
  JOBS=$(nproc)
elif [[ "$OSTYPE" == darwin* ]]; then
  JOBS=$(sysctl -n hw.ncpu)
else
  JOBS=1
fi

# ─── Prepare CMake arguments ───────────────────────────────────────────────────
# start with the core bits
cmake_args=(
  -S . -B build
  -DCMAKE_BUILD_TYPE=Debug
  -G Ninja
)

# ─── Conditionally enable ccache launcher ─────────────────────────────────────
if command -v ccache &> /dev/null; then
  echo "→ ccache found, enabling compiler launcher"
  cmake_args+=(
    -DCMAKE_C_COMPILER_LAUNCHER=ccache
    -DCMAKE_CXX_COMPILER_LAUNCHER=ccache
  )
else
  echo "→ ccache not found, building without it"
fi

# ─── 1) Configure (only if never configured) ───────────────────────────────────
if [ ! -f build/CMakeCache.txt ]; then
  echo "→ Configuring project (fresh build)…"
  cmake "${cmake_args[@]}"
fi

# ─── 2) Incremental build ─────────────────────────────────────────────────────
echo "→ Building (using $JOBS cores)…"
cmake --build build --parallel "$JOBS"

# ─── 3) Run ────────────────────────────────────────────────────────────────────
echo "→ Running…"
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