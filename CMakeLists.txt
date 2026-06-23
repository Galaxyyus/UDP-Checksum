cmake_minimum_required(VERSION 3.15)
project(UDP_Checksum VERSION 1.0.0 LANGUAGES CXX)

# Force C++17 or higher standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Define the executable and its source files
add_executable(udp_checksum
    src/main.cpp
    src/utilities.cpp
)

# Specify include directories for the target
target_include_directories(udp_checksum PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)