project(lilac-core)

file(GLOB_RECURSE SOURCES src/*.cxx)
file(GLOB_RECURSE HEADERS *.h)

add_llvm_library(
        lilac-core
        SHARED
        ${SOURCES}
        ${HEADERS}
)
target_include_directories(lilac-core PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_options(
        lilac-core
        PRIVATE
        -rdynamic
)