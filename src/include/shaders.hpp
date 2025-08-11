#pragma once

inline const constexpr char* cube_vertex_shader_source{
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aNormal;\n"
    "uniform mat4 model;\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    "out vec3 Normal;\n"
    "out vec3 FragPos;\n"
    "void main(){\n"
    "FragPos = vec3(model * vec4(aPos, 1.0));\n"
    "Normal = mat3(transpose(inverse(model)))* aNormal;\n"
    "gl_Position = projection * view * model * vec4(aPos, 1.0);}\n"
};

inline constexpr std::array<const char*, 3> cube_vertex_shader_uniforms{{
    "model", "view", "projection"
}};

inline const constexpr char* cube_fragment_shader_source{
    "#version 330 core\n"
    "in vec3 Normal;\n"
    "in vec3 FragPos;\n"
    "out vec4 FragColor;\n"
    "uniform vec3 objectColor;\n"
    "uniform vec3 lightColor;\n"
    "uniform vec3 lightPos;\n"
    "uniform vec3 viewPos;\n"
    "void main(){\n"
    "float specularStrength = 0.8;\n"
    "vec3 viewDir = normalize(viewPos - FragPos);\n"
    "float ambientStrength = 0.1;\n"
    "vec3 ambient = ambientStrength * lightColor;\n"
    "vec3 norm = normalize(Normal);\n"
    "vec3 lightDir = normalize(lightPos - FragPos);\n"
    "vec3 reflectDir = reflect(-lightDir, norm);\n"
    "float diff = max(dot(norm, lightDir), 0.0);\n"
    "float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);\n"
    "vec3 specular = specularStrength * spec * lightColor;\n"
    "vec3 diffuse = diff * lightColor;\n"
    "vec3 result = (ambient + diffuse + specular) * objectColor;\n"
    "FragColor = vec4(result, 1.0);}\n"
};

inline constexpr std::array<const char*, 4> cube_fragment_shader_uniforms{{
    "objectColor", "lightColor", "lightPos", "viewPos"
}};

enum Cube_Uniforms{
    MODEL,
    VIEW,
    PROJECTION,
    OBJECT_COLOR,
    LIGHT_COLOR,
    LIGHT_POS,
    VIEW_POS
};

inline const constexpr char* light_fragment_shader_source{
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main(){\n"
    "FragColor = vec4(1.0);}\n"
};


