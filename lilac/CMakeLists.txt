cmake_minimum_required(VERSION 3.26)
project(lilac)

set(CMAKE_CXX_STANDARD 26)

file(GLOB_RECURSE SOURCES src/*.cxx)
file(GLOB_RECURSE HEADERS inc/*.h)

add_library(lilac SHARED ${SOURCES} ${HEADERS})
target_compile_options(lilac PRIVATE -fno-rtti)
target_include_directories(lilac PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/inc)
