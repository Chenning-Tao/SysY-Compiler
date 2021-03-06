cmake_minimum_required(VERSION 3.19)
project(compiler)

set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_FLAGS -g)

# find required package
find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)
find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# lexer/parser input filename
file(GLOB_RECURSE L_SOURCES "frontend/*.l")
file(GLOB_RECURSE Y_SOURCES "frontend/*.y")

# lexer/parser output filename
string(REGEX REPLACE ".*/(.*)\\.l" "${CMAKE_CURRENT_BINARY_DIR}/\\1.lex.cpp" L_OUTPUTS "${L_SOURCES}")
string(REGEX REPLACE ".*/(.*)\\.y" "${CMAKE_CURRENT_BINARY_DIR}/\\1.tab.cpp" Y_OUTPUTS "${Y_SOURCES}")

# generate
flex_target(Lexer ${L_SOURCES} ${L_OUTPUTS})
bison_target(Parser ${Y_SOURCES} ${Y_OUTPUTS})
add_flex_bison_dependency(Lexer Parser)

# project include dir
include_directories(${LLVM_INCLUDE_DIRS})
include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/frontend)
include_directories(${PROJECT_SOURCE_DIR}/backend)

separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})
include_directories(${LLVM_INCLUDE_DIRS})


add_executable(compiler ${BISON_Parser_OUTPUTS} ${FLEX_Lexer_OUTPUTS} main.cpp frontend/ast.cpp backend/gen.cpp backend/gen.hpp backend/symbolTable.cpp backend/symbolTable.hpp frontend/macro.cpp frontend/macro.hpp)

llvm_map_components_to_libnames(llvm_libs ${LLVM_TARGETS_TO_BUILD} support core irreader codegen mc mcparser option)

target_link_libraries(compiler ${llvm_libs})
