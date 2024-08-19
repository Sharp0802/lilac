project(lilac-csharp-backend)
set(VERSION 0.0.1)

file(GLOB_RECURSE SOURCES src/*.cxx)
file(GLOB_RECURSE HEADERS *.h)

add_library(lilac-csharp-backend SHARED ${SOURCES} ${HEADERS})
target_link_libraries(lilac-csharp-backend PUBLIC lilac-shared frxml)
target_precompile_headers(lilac-csharp-backend PRIVATE pch.h)
target_include_directories(lilac-csharp-backend PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
