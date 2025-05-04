# clean up old build and any caching
rm -rf build
rm -rf python/src/__pycache__

# cmake build
cmake -S . -B build
cmake --build build

# run executable
./build/portfolio_plusplus