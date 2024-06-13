project(lilac-shared)

file(GLOB_RECURSE SOURCES src/*.cxx)
file(GLOB_RECURSE HEADERS *.h)

add_llvm_library(lilac-shared STATIC ${SOURCES} ${HEADERS})
target_include_directories(lilac-shared PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
