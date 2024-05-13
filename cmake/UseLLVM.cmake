
find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION} ${LLVM_BUILD_TYPE} at '${LLVM_DIR}'")

list(APPEND CMAKE_MODULE_PATH ${LLVM_DIR})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})
include_directories(SYSTEM ${LLVM_INCLUDE_DIRS})
llvm_map_components_to_libnames(LLVM_LIBS Core Support IRReader)

include(AddLLVM)
