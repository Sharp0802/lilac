project(lilac-nm)
set(VERSION 0.0.1)

file(GLOB_RECURSE SOURCES src/*.cxx)
file(GLOB_RECURSE HEADERS *.h)

add_executable(lilac-nm ${SOURCES} ${HEADERS})
target_precompile_headers(lilac-nm PRIVATE pch.h)
target_include_directories(lilac-nm PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(lilac-nm PRIVATE lilac-shared clang-cpp ${LLVM_LIBS})
