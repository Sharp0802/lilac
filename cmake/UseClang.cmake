if (NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    execute_process(
            COMMAND $ENV{SHELL} -c "which clang"
            OUTPUT_STRIP_TRAILING_WHITESPACE
            OUTPUT_VARIABLE CLANG_C_EXE)
    execute_process(
            COMMAND $ENV{SHELL} -c "which clang++"
            OUTPUT_STRIP_TRAILING_WHITESPACE
            OUTPUT_VARIABLE CLANG_CXX_EXE)

    set(CMAKE_C_COMPILER "${CLANG_C_EXE}")
    set(CMAKE_CXX_COMPILER "${CLANG_CXX_EXE}")
endif ()