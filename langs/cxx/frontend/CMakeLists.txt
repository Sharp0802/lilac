project(lilac-cxx-frontend)

file(GLOB_RECURSE SOURCES src/*.cxx)
file(GLOB_RECURSE HEADERS *.h)

add_library(lilac-cxx-frontend ${LIBRARY_LINKAGE} ${SOURCES} ${HEADERS})
target_link_libraries(lilac-cxx-frontend PUBLIC frxml lilac-shared)
target_precompile_headers(lilac-cxx-frontend PRIVATE pch.h)
target_include_directories(lilac-cxx-frontend PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
