add_subdirectory(glad)

add_executable(${CMAKE_PROJECT_NAME}
    main.cpp
)

target_compile_features(${CMAKE_PROJECT_NAME} PRIVATE cxx_std_23)
target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE 

)

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE 
    include_dirs
    glfw
    glad
)

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    include
)

