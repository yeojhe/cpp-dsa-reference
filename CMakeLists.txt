
cmake_minimum_required(VERSION 3.15)
project(cpp_dsa_reference LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
add_compile_options(-Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion)
include_directories(${CMAKE_SOURCE_DIR})
add_executable(strings_section src/strings/strings_section.cpp
        utils.hpp)

add_executable(strings_handrolled
        src/strings/strings_handrolled_demo.cpp
        src/strings/String.cpp
        src/strings/string_utils.h
        src/strings/String.hpp
        src/strings/string_utils.cpp
)