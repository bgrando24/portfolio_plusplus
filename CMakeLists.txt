cmake_minimum_required(VERSION 3.10)
project(portfolio_plusplus)

# include python headers
include_directories(/Users/brandongrando/miniforge3/include/python3.9)

# # pybind: https://pybind11.readthedocs.io/en/latest/compiling.html#modules-with-cmake
# Turn on pybind11’s Python-finder
set(PYBIND11_FINDPYTHON ON CACHE BOOL "" FORCE)
# Point it at *your* venv’s python
set(PYBIND11_FINDPYTHON_EXECUTABLE
    "${CMAKE_SOURCE_DIR}/python/bin/python3"
    CACHE FILEPATH "" FORCE
)
add_subdirectory(deps/pybind11)

add_executable(portfolio_plusplus src/main.cpp)

# Link pybind11 and python libraries
target_link_libraries(portfolio_plusplus PRIVATE pybind11::embed)