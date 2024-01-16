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

set(_SOURCE_FILES_1
    CodeClipPlugin.cpp
    )

set(_SOURCE_FILES_2
    PragmaHandler.cpp
    )

# 添加 Clang 插件的共享库
add_library(code-clip-plugin MODULE ${SOURCE_FILES_1})

# 链接 LLVM 库
llvm_map_components_to_libnames(llvm_libs support core irreader)
target_link_libraries(code-clip-plugin ${llvm_libs})

# 设置插件的输出路径
set_target_properties(code-clip-plugin PROPERTIES
  LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins
)

add_llvm_library(PragmaPlugin MODULE
                 ${_SOURCE_FILES_2}
                 PLUGIN_TOOL clang)

