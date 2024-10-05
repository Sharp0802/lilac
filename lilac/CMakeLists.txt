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
target_link_libraries(lilac PRIVATE clang-cpp frxml lilac-shared)
target_precompile_headers(lilac PRIVATE pch.h)
target_include_directories(lilac PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
add_dependencies(lilac ${BACKENDS} ${FRONTENDS})

if (LIBRARY_LINKAGE STREQUAL "STATIC")
    target_link_options(lilac PRIVATE -Wl,--whole-archive)
    foreach (X IN LISTS BACKENDS)
        target_link_options(lilac PRIVATE $<TARGET_FILE:${X}>)
    endforeach ()
    foreach (X IN LISTS FRONTENDS)
        target_link_options(lilac PRIVATE $<TARGET_FILE:${X}>)
    endforeach ()
    target_link_options(lilac PRIVATE -Wl,--no-whole-archive)
else ()
    foreach (X IN LISTS BACKENDS)
        target_link_libraries(lilac PRIVATE ${X})
    endforeach ()
    foreach (X IN LISTS FRONTENDS)
        target_link_libraries(lilac PRIVATE ${X})
    endforeach ()
endif()