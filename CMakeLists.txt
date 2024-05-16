cmake_minimum_required(VERSION 3.26)
project(lilac)

set(CMAKE_CXX_STANDARD 26)

set(CMAKE_C_COMPILER   /usr/bin/clang)
set(CMAKE_CXX_COMPILER /usr/bin/clang++)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(UseLLVM)

include_directories(${CMAKE_SOURCE_DIR})

add_subdirectory(lilac-core)
add_subdirectory(lilac-cxx)
add_subdirectory(lilac-llvm)
add_subdirectory(lilac-msil)
