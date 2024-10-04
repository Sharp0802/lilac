project(lilac)

set(BACKENDS
        lilac-csharp-backend
)

set(FRONTENDS
        lilac-cxx-frontend
)

file(GLOB_RECURSE SOURCES src/*.cxx)
file(GLOB_RECURSE HEADERS *.h)

add_llvm_executable(lilac ${SOURCES} ${HEADERS})
target_link_libraries(lilac PRIVATE clang-cpp frxml lilac-shared ${BACKENDS} ${FRONTENDS})
target_precompile_headers(lilac PRIVATE pch.h)
target_include_directories(lilac PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
