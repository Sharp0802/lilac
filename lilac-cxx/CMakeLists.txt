project(lilac-cxx)

file(GLOB_RECURSE SOURCES src/*.cxx)
file(GLOB_RECURSE HEADERS *.h)

add_executable(
        lilac-cxx
        ${SOURCES}
        ${HEADERS}
)
target_include_directories(lilac-cxx PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(
        lilac-cxx
        PRIVATE
        lilac-core
        clang-cpp
        ${LLVM_LIBS}
)
