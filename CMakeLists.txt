cmake_minimum_required(VERSION 3.10)
project(project)

# Require C++17 (needed for std::filesystem)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find OpenCV package
# Install on Ubuntu/Debian: sudo apt install libopencv-dev
find_package(OpenCV REQUIRED)

# Include directories
include_directories(${OpenCV_INCLUDE_DIRS} src)

# Automatically collect all .cpp files in src/
# NOTE: If you add a new .cpp file, re-run cmake to pick it up.
file(GLOB_RECURSE SOURCE_FILES src/*.cpp)

# Target executable
add_executable(project ${SOURCE_FILES})

# Link OpenCV libraries (and stdc++fs for filesystem on older GCC)
target_link_libraries(project ${OpenCV_LIBS})
