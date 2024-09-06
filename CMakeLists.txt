cmake_minimum_required(VERSION 3.26)
project(lilac)



set(LILAC_TARGETS "ild;lilac-csharp;lilac-cxx" CACHE STRING "Control which targets are enabled")



set(ENABLE_LLVM_SHARED ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

include(UseClang)
include(UseLLVM)

set(LLVM_OPTIMIZED_TABLEGEN ON)
set(CMAKE_CXX_STANDARD 26)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build/bin/")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build/lib/")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build/dyn/")

add_subdirectory(ild)
add_subdirectory(langs)
add_subdirectory(sub/frxml/frxml)


add_custom_target(lilac ALL DEPENDS ${LILAC_TARGETS})
