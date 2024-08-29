cmake_minimum_required(VERSION 3.5)
project(test)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(test SHARED cxx/test.cxx)
