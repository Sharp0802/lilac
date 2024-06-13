cmake_minimum_required(VERSION 3.26)
project(lilac)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

include(UseClang)
include(UseLLVM)

set(LLVM_OPTIMIZED_TABLEGEN ON)
set(CMAKE_CXX_STANDARD 26)

include_directories(${CMAKE_SOURCE_DIR})

add_subdirectory(lilac-core)
add_subdirectory(lilac-llvm)
add_subdirectory(langs)
