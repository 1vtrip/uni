cmake_minimum_required(VERSION 3.16)
project(RepairFirm CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)
add_executable(repair_firm ${SOURCES})
target_include_directories(repair_firm PRIVATE src)
target_compile_options(repair_firm PRIVATE -Wall -Wextra)
