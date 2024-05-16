project(lilac-llvm)
set(VERSION 0.0.1)

file(GLOB_RECURSE SOURCES src/*.cxx)
file(GLOB_RECURSE HEADERS *.h)

string(TIMESTAMP TIMESTAMP "%Y%m%d")
configure_file(version.h.in version.h)

add_executable(
        lilac-llvm
        ${SOURCES}
        ${HEADERS}
)
target_precompile_headers(
        lilac-llvm
        PRIVATE
        pch.h
)
target_include_directories(
        lilac-llvm
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
)
target_link_libraries(
        lilac-llvm
        PRIVATE
        lilac-core
        clang-cpp
        ${LLVM_LIBS}
)
