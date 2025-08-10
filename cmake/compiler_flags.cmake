
set(extra_flags "" CACHE STRING "")
add_library(warning_flags INTERFACE)
target_compile_options(warning_flags INTERFACE
    /Wall
    /permissive-
    ${extra_flags}
)
message(${CMAKE_CXX_FLAGS})
message(${CMAKE_CXX_FLAGS_DEBUG})
