project(lilac-csharp-backend)

file(GLOB_RECURSE SOURCES src/*.cxx)
file(GLOB_RECURSE HEADERS *.h)

add_lilac_module(lilac-csharp-backend)
target_include_directories(lilac-csharp-backend PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
target_sources(lilac-csharp-backend PRIVATE ${SOURCES} ${HEADERS})
target_precompile_headers(lilac-csharp-backend PRIVATE pch.h)
