cmake_minimum_required(VERSION 3.26)
project(lilac)

set(CMAKE_CXX_STANDARD 26)

file(GLOB_RECURSE SOURCES "src/*.cxx")
file(GLOB_RECURSE HEADERS "inc/*.h")

message(${SOURCES})

foreach (HEADER ${HEADERS})
    file(RELATIVE_PATH HEADER_PATH "${CMAKE_CURRENT_SOURCE_DIR}" "${HEADER}")
    set(INCLUDES "${INCLUDES}#include \"${HEADER_PATH}\"\n")
endforeach ()

configure_file(lilac.h.in lilac.h)

add_library(lilac SHARED ${SOURCES} ${HEADERS} lilac.h)
target_compile_options(lilac PRIVATE -fno-rtti)
target_include_directories(lilac PRIVATE
        inc
    ${CMAKE_CURRENT_BINARY_DIR}
)
