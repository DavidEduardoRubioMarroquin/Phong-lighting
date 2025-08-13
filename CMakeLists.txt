cmake_minimum_required(VERSION 3.26)
project(babysFirstLightGL C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(./cmake/compiler_flags.cmake)
include(./cmake/library_setup.cmake)

add_subdirectory(src)