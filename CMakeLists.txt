cmake_minimum_required(VERSION 3.10)
project(portfolio_plusplus)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# enforce real C++17, not GNU extensions
set(CMAKE_CXX_EXTENSIONS OFF) 

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
add_subdirectory(extern/pybind11)

add_subdirectory(extern/drogon)

set(Drogon_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(Drogon_BUILD_TESTS    OFF CACHE BOOL "" FORCE)

file(GLOB CONTROLLERS_SRC src/http_controllers/*.cpp)
add_executable(portfolio_plusplus src/main.cpp ${CONTROLLERS_SRC})

# Link pybind11 and python libraries
target_link_libraries(portfolio_plusplus PRIVATE pybind11::embed)

# Link it to drogon:
target_link_libraries(portfolio_plusplus PRIVATE drogon)