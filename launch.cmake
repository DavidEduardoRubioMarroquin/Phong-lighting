set(exe_path ${CMAKE_CURRENT_LIST_DIR}/build/ms_ninja_deb/src/babysFirstLightGL.exe)
execute_process(
    OUTPUT_STRIP_TRAILING_WHITESPACE
    COMMAND ${exe_path}
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/src/
    OUTPUT_VARIABLE output_var
)

if(output_var)
message(${output_var})
endif()