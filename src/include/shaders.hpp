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

inline constexpr std::array<const char *, 4> cube_vertex_shader_uniforms{{
    "model", "view", "projection", "lightPos"
}};

inline const constexpr char* cube_fragment_shader_source{R"(
    #version 330 core
    struct Material {
        vec3 ambient;
        vec3 diffuse;
        vec3 specular;
        float shininess;
    };
    struct Light {
        vec3 ambient;
        vec3 diffuse;
        vec3 specular;
    };

    in vec3 Normal;
    in vec3 FragPos;
    in vec3 LightPos;

    uniform Material material;
    uniform Light light;

    out vec4 FragColor;
    void main(){

        //ambient
        vec3 ambient = material.ambient * light.ambient;

        //diffuse
        vec3 norm = normalize(Normal);
        vec3 lightDir = normalize(LightPos - FragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = (material.diffuse * diff) * light.diffuse;
        
        //specular
        vec3 viewDir = normalize(-FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        vec3 specular = (material.specular * spec) * light.specular;

        vec3 result = ambient + diffuse + specular;
        FragColor = vec4(result, 1.0);
    }
)"};

inline constexpr std::array<const char *, 7> cube_fragment_shader_uniforms{{
    "material.ambient", "material.diffuse", "material.specular", "material.shininess",
    "light.ambient", "light.diffuse", "light.specular"
}};

inline const constexpr char* light_fragment_shader_source{R"(
    #version 330 core

    uniform vec3 LightColor;

    out vec4 FragColor;
    void main(){
        FragColor = vec4(LightColor, 1.0);
    }
)"};

inline constexpr std::array<const char *, 1> light_fragment_shader_uniforms{{
    "LightColor"
}};



