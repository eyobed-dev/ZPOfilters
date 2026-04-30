cmake_minimum_required(VERSION 3.10)
project(ZPOe_Filter_Effects)

# Find OpenCV package
find_package(OpenCV REQUIRED)

# Include directories
include_directories(${OpenCV_INCLUDE_DIRS} include)

# Source files
set(SOURCE_FILES src/main.cpp)

# Target executable
add_executable(ZPOe_Filter_Effects ${SOURCE_FILES})

# Link OpenCV libraries
target_link_libraries(ZPOe_Filter_Effects ${OpenCV_LIBS})
