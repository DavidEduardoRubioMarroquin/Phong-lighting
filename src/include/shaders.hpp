#pragma once

inline const constexpr char* cube_vertex_shader_source{R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aNormal;

    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;
    uniform vec3 lightPos;

    out vec3 Normal;
    out vec3 FragPos;
    out vec3 LightPos;
    void main(){
        FragPos = vec3(view * model * vec4(aPos, 1.0));
        LightPos = vec3(view * vec4(lightPos, 1.0));
        Normal = mat3(transpose(inverse(view * model)))* aNormal;
        gl_Position = projection * view * model * vec4(aPos, 1.0);
    }
)"};

inline constexpr std::array<const char*, 4> cube_vertex_shader_uniforms{{
    "model", "view", "projection", "lightPos"
}};

inline const constexpr char* cube_fragment_shader_source{R"(
    #version 330 core
    in vec3 Normal;
    in vec3 FragPos;
    in vec3 LightPos;

    uniform vec3 objectColor;
    uniform vec3 lightColor;

    out vec4 FragColor;
    void main(){
        //constants 
        float specularStrength = 0.9;
        float ambientStrength = 0.05;
        float diffuseStrength = 0.5;

        //ambient
        vec3 ambient = ambientStrength * lightColor;

        //diffuse
        vec3 norm = normalize(Normal);
        vec3 lightDir = normalize(LightPos - FragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diffuseStrength * diff * lightColor;
        
        //specular
        vec3 viewDir = normalize(-FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 8);

        vec3 specular = specularStrength * spec * lightColor;
        vec3 result = (ambient + diffuse + specular) * objectColor;
        FragColor = vec4(result, 1.0);
    }
)"};

inline constexpr std::array<const char*, 2> cube_fragment_shader_uniforms{{
    "objectColor", "lightColor"
}};

enum Cube_Uniforms{
    MODEL,
    VIEW,
    PROJECTION,
    LIGHT_POS,
    OBJECT_COLOR,
    LIGHT_COLOR,
};

inline const constexpr char* light_fragment_shader_source{
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main(){\n"
    "FragColor = vec4(1.0);}\n"
};


