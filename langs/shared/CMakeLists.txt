project(lilac-shared)

file(GLOB_RECURSE SOURCES src/*.cxx)
file(GLOB_RECURSE HEADERS *.h)

add_library(lilac-shared SHARED ${SOURCES} ${HEADERS})
target_include_directories(lilac-shared PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
