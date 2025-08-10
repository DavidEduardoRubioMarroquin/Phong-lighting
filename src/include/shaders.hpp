#pragma once

inline const constexpr char* cube_vertex_shader_source{
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "uniform mat4 model;\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    "void main(){\n"
    "gl_Position = projection * view * model * vec4(aPos, 1.0);}\n"
};

inline const constexpr char* cube_fragment_shader_source{
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "uniform vec3 objectColor;"
    "uniform vec3 lightColor;"
    "void main(){\n"
    "FragColor = vec4(objectColor * lightColor, 1.0);}\n"
};

inline const constexpr char* light_fragment_shader_source{
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main(){\n"
    "FragColor = vec4(1.0);}\n"
};

