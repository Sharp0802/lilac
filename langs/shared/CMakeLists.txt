project(lilac-shared)

file(GLOB_RECURSE SOURCES src/*.cxx)
file(GLOB_RECURSE HEADERS *.h)

add_library(lilac-shared ${LIBRARY_LINKAGE} ${SOURCES} ${HEADERS})
if (LIBRARY_LINKAGE STREQUAL "STATIC")
    target_include_directories(lilac-shared PUBLIC $<TARGET_PROPERTY:frxml,INCLUDE_DIRECTORIES>)
else ()
    target_link_libraries(lilac-shared PUBLIC frxml clang-cpp)
endif ()
target_include_directories(lilac-shared PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
