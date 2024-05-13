project(lilac-cxx)
set(VERSION 0.0.1)

file(GLOB_RECURSE SOURCES src/*.cxx)
file(GLOB_RECURSE HEADERS *.h)

string(TIMESTAMP TIMESTAMP "%Y%m%d")
configure_file(version.h.in version.h)

add_executable(
        lilac-cxx
        ${SOURCES}
        ${HEADERS}
)
target_precompile_headers(
        lilac-cxx
        PRIVATE
        pch.h
)
target_include_directories(
        lilac-cxx
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
)
target_link_libraries(
        lilac-cxx
        PRIVATE
        lilac-core
        clang-cpp
        ${LLVM_LIBS}
)
