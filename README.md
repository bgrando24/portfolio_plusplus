# Portfolio++
A simple personal stock portfolio tracker and visualiser

## Get started
cmake -S . -B build
cmake --build build

#### Python virtual env
python -m venv python
source python/bin/activate
(pip list to check active packages)

### yfinance python package
yfinance: https://github.com/ranaroussi/yfinance

### pybind submodule - allows running python in c++
pybind (as git submodule): https://pybind11.readthedocs.io/en/latest/installing.html

### Drogon - C++ HTTP server framework
drogon add as submodule:
git submodule add https://github.com/drogonframework/drogon.git extern/drogon
git submodule update --init --recursive

### Ninja - C++ build system
https://ninja-build.org/
brew install ninja