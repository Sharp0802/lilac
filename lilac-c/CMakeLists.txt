cmake_minimum_required(VERSION 3.26)
project(lilac-c)

set(CMAKE_CXX_STANDARD 26)

set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_C_COMPILER clang)

find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION} ${LLVM_BUILD_TYPE} at '${LLVM_DIR}'")
list(APPEND CMAKE_MODULE_PATH ${LLVM_DIR})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})
include_directories(SYSTEM ${LLVM_INCLUDE_DIRS})
llvm_map_components_to_libnames(LLVM_LIBS Core)

file(GLOB_RECURSE SOURCES src/*.cxx)
file(GLOB_RECURSE HEADERS inc/*.h)

include(AddLLVM)

add_llvm_executable(lilac-c ${SOURCES} ${HEADERS})
target_include_directories(lilac-c PRIVATE inc/)
target_compile_options(lilac-c PRIVATE -fno-rtti)
target_link_libraries(lilac-c PRIVATE ${LLVM_LIBS} clang-cpp)
target_link_options(lilac-c PRIVATE -rdynamic)
