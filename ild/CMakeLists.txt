project(ild)

set(BACKENDS
        lilac-csharp-backend
)

file(GLOB_RECURSE SOURCES src/*.cxx)
file(GLOB_RECURSE HEADERS *.h)

add_llvm_executable(ild ${SOURCES} ${HEADERS})
target_link_libraries(ild PRIVATE clang-cpp ${BACKENDS})
target_precompile_headers(ild PRIVATE pch.h)
target_include_directories(ild PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
