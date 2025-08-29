#pragma once

inline const constexpr char* cube_vertex_shader_source{R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aNormal;
    layout (location = 2) in vec2 aTexCoords;

    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;

    out vec3 Normal;
    out vec3 FragPos;
    out vec2 TexCoords;
    void main(){
        FragPos = vec3(model * vec4(aPos, 1.0));
        Normal = mat3(transpose(inverse(model)))* aNormal;
        gl_Position = projection * view * model * vec4(aPos, 1.0);

        TexCoords = aTexCoords;
    }
)"};

inline constexpr std::array<const char *, 3> cube_vertex_shader_uniforms{{
    "model", "view", "projection"
}};

inline const constexpr char* cube_fragment_shader_source{R"(
    #version 330 core

    struct Material {
        sampler2D diffuse_map;
        sampler2D specular_map;
        sampler2D emission_map;
        float shininess;
    };

    struct Light {
        vec3 direction;

        vec3 ambient;
        vec3 diffuse;
        vec3 specular;
    };

    in vec3 Normal;
    in vec3 FragPos;
    in vec2 TexCoords;

    uniform vec3 viewPos;
    uniform Material material;
    uniform Light light;

    out vec4 FragColor;
    void main(){
        vec3 norm = normalize(Normal);
        vec3 lightDir = normalize(-light.direction); //lightDir points from object to light
        float diff = max(dot(norm, lightDir), 0.0);
        
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

        vec3 ambient  = light.ambient  * vec3(texture(material.diffuse_map, TexCoords));
        vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse_map, TexCoords));  
        vec3 specular = light.specular * spec * vec3(texture(material.specular_map, TexCoords));
        vec3 emission = texture(material.emission_map, TexCoords).rgb;
        FragColor = vec4(ambient + diffuse + specular, 1.0);   
    }
)"};

inline constexpr std::array<const char*, 9> cube_fragment_shader_uniforms{{
    "material.diffuse_map", "material.specular_map", "material.shininess", "material.emission_map",
    "light.ambient", "light.diffuse", "light.specular", "light.direction", "viewPos"
}};

inline const constexpr char* light_fragment_shader_source{R"(
    #version 330 core

    uniform vec3 LightColor;

    out vec4 FragColor;
    void main(){
        FragColor = vec4(LightColor, 1.0);
    }
)"};

inline constexpr std::array<const char*, 1> light_fragment_shader_uniforms{{
    "LightColor"
}};



