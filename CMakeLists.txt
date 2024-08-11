cmake_minimum_required(VERSION 3.26)
project(lilac)

set(ENABLE_LLVM_SHARED ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

include(UseClang)
include(UseLLVM)

set(LLVM_OPTIMIZED_TABLEGEN ON)
set(CMAKE_CXX_STANDARD 26)

include_directories(${CMAKE_SOURCE_DIR})

add_subdirectory(sub/frxml/frxml)
add_subdirectory(langs)
