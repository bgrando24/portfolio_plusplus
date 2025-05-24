cmake_minimum_required(VERSION 3.10) # Consider 3.12+ if pybind11 recommends for PYBIND11_FINDPYTHON
project(portfolio_plusplus)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF) 

# --- Pybind11 Configuration ---
# Tell pybind11 to use its modern Python discovery mechanism
set(PYBIND11_FINDPYTHON ON CACHE BOOL "Enable pybind11's Python discovery" FORCE)

# Explicitly point pybind11 to your virtual environment's Python interpreter
# This interpreter will be queried by pybind11 to find includes, libraries, etc.
set(PYBIND11_PYTHON_EXECUTABLE
    "${CMAKE_SOURCE_DIR}/python/bin/python3"
    CACHE FILEPATH "Path to Python executable in the virtual environment" FORCE
)
# Note: PYBIND11_FINDPYTHON_EXECUTABLE is an older variable name,
# PYBIND11_PYTHON_EXECUTABLE is preferred for pybind11 v2.7+
# If using an older pybind11, stick to PYBIND11_FINDPYTHON_EXECUTABLE.
# Given your submodule is 'stable', it's likely recent enough for PYBIND11_PYTHON_EXECUTABLE.
# If unsure, you can set both or check your pybind11 version/docs.
# For safety with potentially older stable branch:
set(PYBIND11_PYTHON_EXECUTABLE ${PYBIND11_PYTHON_EXECUTABLE} CACHE FILEPATH "" FORCE)


# Add pybind11 subdirectory. This will define the pybind11::* targets.
add_subdirectory(extern/pybind11)

# --- Drogon Configuration ---
add_subdirectory(extern/drogon)
set(Drogon_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(Drogon_BUILD_TESTS    OFF CACHE BOOL "" FORCE)

# --- Project Sources ---
file(GLOB_RECURSE SRC_FILES src/**/*.cpp) # Renamed SRC to SRC_FILES for clarity

add_executable(portfolio_plusplus src/main.cpp ${SRC_FILES})

# --- Target Include Directories ---
# Only project-specific include directories here.
# Python includes will come from pybind11::embed.
target_include_directories(portfolio_plusplus PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# --- Target Link Libraries ---
# pybind11::embed handles Python embedding (includes, libraries, flags)
target_link_libraries(portfolio_plusplus PRIVATE 
    pybind11::embed 
    drogon
)

# --- Optional: Print Python info found by pybind11 for verification ---
# This requires pybind11 to have processed its Python discovery.
# This message will appear during the CMake configuration step.
if(TARGET pybind11::embed)
    get_target_property(PYBIND11_INCLUDE_DIRS pybind11::embed INTERFACE_INCLUDE_DIRECTORIES)
    # INTERFACE_LINK_LIBRARIES might also list the Python library if pybind11 exposes it this way
    message(STATUS "pybind11::embed will use Python include directories: ${PYBIND11_INCLUDE_DIRS}")
    # You might need to check pybind11's specific CMake variables to see the Python library path it found.
    # For example, pybind11 might set Python_LIBRARIES internally.
endif()
if(Python_FOUND)
    message(STATUS "CMake's FindPython (potentially used by pybind11) found: ")
    message(STATUS "  Interpreter: ${Python_EXECUTABLE}")
    message(STATUS "  Libraries: ${Python_LIBRARIES}")
    message(STATUS "  Include Dirs: ${Python_INCLUDE_DIRS}")
    message(STATUS "  Version: ${Python_VERSION_STRING}")
endif()