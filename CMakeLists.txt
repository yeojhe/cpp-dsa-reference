
cmake_minimum_required(VERSION 3.15)
project(cpp_dsa_reference LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
add_compile_options(-Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion)
include_directories(${CMAKE_SOURCE_DIR})
add_executable(strings_section src/strings_section.cpp
        utils.hpp)