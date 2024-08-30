project(lilac-cxx)

file(GLOB_RECURSE SOURCES src/*.cxx)
file(GLOB_RECURSE HEADERS *.h)

add_llvm_executable(lilac-cxx ${SOURCES} ${HEADERS})
target_link_libraries(lilac-cxx PUBLIC clang-cpp frxml lilac-shared)
target_precompile_headers(lilac-cxx PRIVATE pch.h)
target_include_directories(lilac-cxx PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
