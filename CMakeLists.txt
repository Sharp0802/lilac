cmake_minimum_required(VERSION 3.26)
project(lilac)

set(ENABLE_LLVM_SHARED ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

include(UseClang)
include(UseLLVM)

set(LLVM_OPTIMIZED_TABLEGEN ON)
set(CMAKE_CXX_STANDARD 26)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include_directories(${CMAKE_SOURCE_DIR})

add_subdirectory(langs)
add_subdirectory(sub/frxml/frxml)
