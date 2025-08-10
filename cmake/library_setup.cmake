#set to correct value
set(glm_include_path C:/Libraries/Cpp/glm-1.0.1-light/ CACHE STRING "" FORCE)
set(glfw_src_path C:/Libraries/Cpp/glfw-3.4/ CACHE STRING "" FORCE)

#glm is header only library
add_library(include_dirs INTERFACE)
target_include_directories(include_dirs INTERFACE
    ${glm_include_path}
)

#glfw is compiled with projct
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLE OFF CACHE BOOL "" FORCE)
add_subdirectory(${glfw_src_path} ${CMAKE_BINARY_DIR}/glfw3)