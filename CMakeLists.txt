cmake_minimum_required(VERSION 3.5)
project(code-clip)

#设置C++标志
set(CMAKE_CXX_STANDARD 17)

# 查找 LLVM
find_package(LLVM REQUIRED CONFIG)

message(STATUS "Using LLVM ${LLVM_PACKAGE_VERSION}")

# 将 LLVM 目录添加到包含路径中
add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS})
# 添加链接库
link_directories(${LLVM_LIBRARY_DIR})

list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(AddLLVM)

find_package(Clang REQUIRED CONFIG)
include_directories(${CLANG_INCLUDE_DIRS})
#list(APPEND CMAKE_MODULE_PATH "${CLANG_CMAKE_DIR}")

# Handle RTTI stuff, which often leads to error
if(NOT ${LLVM_ENABLE_RTTI})
  if("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang" OR
      "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR
      "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
    message(STATUS "Disable RTTI")
  elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /GR-")
    message(STATUS "Disable RTTI")
  endif()
  # Do not give any flags for other less widely used
  # compilers
endif()

set( LLVM_LINK_COMPONENTS
  ${LLVM_TARGETS_TO_BUILD}
  Option
  Support
  )

add_executable(my_clang_tool CodeClipPlugin.cpp)

target_link_libraries(my_clang_tool PRIVATE clang)
target_include_directories(my_clang_tool PRIVATE ${CLANG_INCLUDE_DIRS})
target_link_libraries(my_clang_tool PRIVATE ${CLANG_LIBS})
target_link_libraries(my_clang_tool PRIVATE clangAST clangRewrite)

target_include_directories(my_clang_tool PRIVATE ${LLVM_INCLUDE_DIRS})
target_compile_definitions(my_clang_tool PRIVATE ${LLVM_DEFINITIONS})

# Link LLVM libraries
llvm_map_components_to_libnames(LLVM_LIBS core support)

target_link_libraries(my_clang_tool PRIVATE ${LLVM_LIBS})

target_link_libraries(my_clang_tool
    PRIVATE
    clangTooling
    clangFrontend
    clangFrontendTool
    clangDriver
    clangSerialization
    clangCodeGen
    clangParse
    clangSema
    clangStaticAnalyzerFrontend
    clangStaticAnalyzerCheckers
    clangStaticAnalyzerCore
    clangAnalysis
    clangARCMigrate
    clangRewrite
    clangRewriteFrontend
    clangEdit
    clangAST
    clangLex
    clangBasic
    clangStaticAnalyzerFrontend
  clangTooling
  clangToolingSyntax
    )