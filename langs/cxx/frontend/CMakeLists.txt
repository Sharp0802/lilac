project(lilac-cxx-frontend)

file(GLOB_RECURSE SOURCES src/*.cxx)
file(GLOB_RECURSE HEADERS *.h)

add_lilac_module(lilac-cxx-frontend)
target_include_directories(lilac-cxx-frontend PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_sources(lilac-cxx-frontend PRIVATE ${SOURCES} ${HEADERS})
target_precompile_headers(lilac-cxx-frontend PRIVATE pch.h)
