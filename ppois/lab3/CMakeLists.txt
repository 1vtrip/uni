cmake_minimum_required(VERSION 3.16)
project(FileManagementSystem CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)
add_executable(fms ${SOURCES})
target_include_directories(fms PRIVATE src)
target_compile_options(fms PRIVATE -Wall -Wextra)
