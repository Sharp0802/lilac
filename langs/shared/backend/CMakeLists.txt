project(lilac-shared)

file(GLOB_RECURSE HEADERS *.h)

add_library(lilac-shared INTERFACE ${HEADERS})
target_include_directories(lilac-shared INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
