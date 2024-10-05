cmake_minimum_required(VERSION 3.26)
project(lilac)


set(LIBRARY_LINKAGE STATIC)

function (add_lilac_module name)
    add_library(${name} ${LIBRARY_LINKAGE})
    if (LIBRARY_LINKAGE STREQUAL "STATIC")
        get_target_property(LILAC_INCLUDE_DIRS lilac-shared INCLUDE_DIRECTORIES)
        target_include_directories(${name} PUBLIC ${LILAC_INCLUDE_DIRS})
    else ()
        target_link_libraries(${name} PUBLIC lilac-shared)
    endif ()
endfunction()


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
link_directories(${CMAKE_ARCHIVE_OUTPUT_DIRECTORY} ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

add_subdirectory(lilac)
add_subdirectory(langs)
add_subdirectory(sub/frxml/frxml)
