#pragma warning(push, 0)
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#pragma warning(disable : 5045)
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#pragma warning(pop)

#include <print>
#include <vector>
#include <unordered_map>
#include <string_view>
#include <array>
#include <span>

#include "camera.hpp"
#include "globals.hpp"
#include "shaders.hpp"

struct Light{
    glm::vec3 position;
    glm::vec3 color;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

Light light{
    {1.2f, 1.0f, 2.0f},
    glm::vec3{1.0f},
    {1.0f, 1.0f, 1.0f},
    {1.0f, 1.0f, 1.0f},
    {1.0f, 1.0f, 1.0f}
};

Camera camera{glm::vec3(0.0f, 1.0f, 2.5f)};
float lastX{Window::g_width/ 2.0f};
float lastY{Window::g_height/ 2.0f};
bool firstMouse{true};

enum VAOs_index{
    CUBE_VAO,
    LIGHT_VAO,
    MAX_VAOS};
std::array<uint32_t, MAX_VAOS> VAOs{};

enum Shader_Program{
    CUBE_SHADER,
    LIGHT_SHADER,
    MAX_SHADER_PROGRAMS};
std::array<uint32_t, MAX_SHADER_PROGRAMS> shader_programs{};
std::array<std::unordered_map<std::string_view, int32_t>, MAX_SHADER_PROGRAMS> shader_uniforms{};

enum Textures{
    CONTAINER_2,
    CONTAINER_2_SPEC,
    CONTAINER_2_EM,
    MAX_TEX
};
std::array<uint32_t, MAX_TEX> texture_ids{};

float deltaTime{0.0f};
float lastFrame{0.0f};

GLFWwindow* setup_GLFW();
void process_input(GLFWwindow*);
void init_buffers();
void init_shader_programs();
void load_textures();
void set_uniform_values();

int main(){
    auto window{setup_GLFW()};

    #pragma warning(push, 0) //type cast safety warning
    if(!gladLoadGLLoader( (GLADloadproc)glfwGetProcAddress )) {
        std::println("failed to initialize glad");
        return -1;
    }
    #pragma warning(pop)
    glEnable(GL_DEPTH_TEST);

    init_buffers();
    init_shader_programs();
    load_textures();
    set_uniform_values();

    while(!glfwWindowShouldClose(window)){
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = lastFrame - currentFrame;
        lastFrame = currentFrame;
        process_input(window);

        glm::mat4 projection{glm::perspective(glm::radians(camera.m_zoom),
        static_cast<float>(Window::g_width)/static_cast<float>(Window::g_height), 0.1f, 100.0f)};
        glm::mat4 view{camera.getViewMatrix()};
        
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        light.position.x = std::sinf(currentFrame * PI/2);
        light.position.z = std::cosf(currentFrame * PI/2);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_ids[CONTAINER_2]);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture_ids[CONTAINER_2_SPEC]);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, texture_ids[CONTAINER_2_EM]);

        glUseProgram(shader_programs[CUBE_SHADER]);
        glUniformMatrix4fv(shader_uniforms[CUBE_SHADER]["view"], 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(shader_uniforms[CUBE_SHADER]["projection"], 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(shader_uniforms[CUBE_SHADER]["model"], 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
        glUniform3fv(shader_uniforms[CUBE_SHADER]["lightPos"], 1, glm::value_ptr(light.position));
        glBindVertexArray(VAOs[CUBE_VAO]);
        glDrawArrays(GL_TRIANGLES, 0 , 36);
        
        glm::mat4 model{glm::mat4(1.0f)};
        model = glm::translate(model, light.position);
        model = glm::scale(model, glm::vec3(0.2f));
        model = glm::rotate(model, currentFrame * PI/2, {0.0, 1.0, 0.0});
        
        glUseProgram(shader_programs[LIGHT_SHADER]);
        glUniformMatrix4fv(shader_uniforms[LIGHT_SHADER]["view"], 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(shader_uniforms[LIGHT_SHADER]["projection"], 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(shader_uniforms[LIGHT_SHADER]["model"], 1, GL_FALSE, glm::value_ptr(model));
        glUniform3fv(shader_uniforms[LIGHT_SHADER]["LightColor"], 1, glm::value_ptr(light.color));
        glBindVertexArray(VAOs[LIGHT_VAO]);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwTerminate();
}

void set_uniform_values(){
    glUseProgram(shader_programs[CUBE_SHADER]);
    glUniform1i(shader_uniforms[CUBE_SHADER]["material.diffuse_map"], 0);
    glUniform1i(shader_uniforms[CUBE_SHADER]["material.specular_map"], 1);
    glUniform1i(shader_uniforms[CUBE_SHADER]["material.emission_map"], 2);
    glUniform1f(shader_uniforms[CUBE_SHADER]["material.shininess"], 32.0f);
    glUniform3fv(shader_uniforms[CUBE_SHADER]["light.ambient"], 1, glm::value_ptr(light.ambient));
    glUniform3fv(shader_uniforms[CUBE_SHADER]["light.diffuse"], 1, glm::value_ptr(light.diffuse));
    glUniform3fv(shader_uniforms[CUBE_SHADER]["light.specular"], 1, glm::value_ptr(light.specular));
    glUseProgram(0); 
}

void fill_uniforms(Shader_Program program, std::span<const char* const> uniforms){
    for(const char* uniform : uniforms){
        shader_uniforms[program].emplace(uniform, glGetUniformLocation(shader_programs[program], uniform));
    }
}

void link_program(Shader_Program program, uint32_t vertex, uint32_t fragment){
    glAttachShader(shader_programs[program], vertex);
    glAttachShader(shader_programs[program], fragment);
    glLinkProgram(shader_programs[program]);

    int32_t success;
    char info_log[512];
    glGetProgramiv(shader_programs[program], GL_LINK_STATUS, &success);
    if(!success){
        glGetProgramInfoLog(shader_programs[program], 512, nullptr, info_log);
        std::println("Failed to link program: \n{}", info_log);
    }
}

uint32_t compile_shader(GLenum type, const char* source){
    uint32_t shader{glCreateShader(type)};
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    int32_t success;
    char info_log[512];

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if(!success){
        glGetShaderInfoLog(shader, 512, nullptr, info_log);
        std::println("Failed to compile shader: \n{}", info_log);
    }
    return shader;
}

void init_shader_programs(){
    uint32_t cube_vertex{compile_shader(GL_VERTEX_SHADER, cube_vertex_shader_source)};
    uint32_t cube_fragment{compile_shader(GL_FRAGMENT_SHADER, cube_fragment_shader_source)};
    uint32_t light_fragment{compile_shader(GL_FRAGMENT_SHADER, light_fragment_shader_source)};

    shader_programs[CUBE_SHADER] = glCreateProgram();
    shader_programs[LIGHT_SHADER] = glCreateProgram();

    link_program(CUBE_SHADER, cube_vertex, cube_fragment);
    link_program(LIGHT_SHADER, cube_vertex, light_fragment);

    fill_uniforms(CUBE_SHADER, cube_vertex_shader_uniforms);
    fill_uniforms(CUBE_SHADER, cube_fragment_shader_uniforms);
    fill_uniforms(LIGHT_SHADER, cube_vertex_shader_uniforms);
    fill_uniforms(LIGHT_SHADER, light_fragment_shader_uniforms);
    
    glDeleteShader(cube_vertex);
    glDeleteShader(cube_fragment);
    glDeleteShader(light_fragment);
}

void init_buffers(){
    enum VBOs{
        CUBE_VERTICES,
        MAX_VBOS
    };
    std::array<uint32_t, MAX_VBOS> VBOs{};

    glGenVertexArrays(MAX_VAOS, VAOs.data());
    glGenBuffers(MAX_VBOS, VBOs.data());

    glBindBuffer(GL_ARRAY_BUFFER, VBOs[CUBE_VERTICES]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

    for(uint32_t VAO : VAOs){
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER,VBOs[CUBE_VERTICES]);
        glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(float) * 8, (void*)0);
        glEnableVertexAttribArray(0);
    }

    glBindVertexArray(VAOs[CUBE_VAO]);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
}

uint32_t load_texture(const char* tex_path){
    uint32_t texture_id;
    glGenTextures(1, &texture_id);

    int32_t width, height, nr_channels;
    uint8_t* data {stbi_load(tex_path, &width, &height, &nr_channels, 0)};
    
    if(data){
        GLenum format{};
        switch(nr_channels){
            case 3: format = GL_RGB; break;
            case 4: format = GL_RGBA; break;
            default: format = GL_RED; break;
        }
        
        glBindTexture(GL_TEXTURE_2D, texture_id);
        glTexImage2D(GL_TEXTURE_2D, 0, static_cast<int32_t>(format), width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    }else{
        std::println("Failed to load texture at ({}).", tex_path);
    }

    stbi_image_free(data);
    return texture_id;
}

void load_textures(){
    stbi_set_flip_vertically_on_load(true);

    texture_ids[CONTAINER_2] = load_texture("textures/container2.png");
    texture_ids[CONTAINER_2_SPEC] = load_texture("textures/container2_specular.png");
    texture_ids[CONTAINER_2_EM] = load_texture("textures/container2_emission.jpg");
}

void process_input(GLFWwindow* window){
    auto pressed = [&window](int32_t glfw_key) -> bool{
        return (glfwGetKey(window, glfw_key) == GLFW_PRESS);
    };

    if(pressed(GLFW_KEY_ESCAPE)){
        glfwSetWindowShouldClose(window, true);
    }
    using enum Camera_Movement;
    if(pressed(GLFW_KEY_W)){
        camera.keyInput(FORWARD, deltaTime);
    }
    if(pressed(GLFW_KEY_S)){
        camera.keyInput(BACKWARD, deltaTime);
    }
    if(pressed(GLFW_KEY_A)){
        camera.keyInput(LEFT, deltaTime);
    }
    if(pressed(GLFW_KEY_D)){
        camera.keyInput(RIGHT, deltaTime);
    }
}

void mouse_callback([[maybe_unused]]GLFWwindow* window, double xpos_in, double ypos_in) noexcept{
    float xpos = static_cast<float>(xpos_in);
    float ypos = static_cast<float>(ypos_in);

    if(firstMouse){
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; //y cordinates are inverted

    lastX = xpos;
    lastY = ypos;

    camera.mouseInput(xoffset, yoffset);
}

void scroll_callback([[maybe_unused]]GLFWwindow* window, double xoffset,[[maybe_unused]] double yoffset) noexcept{
    camera.scrollInput(static_cast<float>(xoffset));
}

GLFWwindow* setup_GLFW(){
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow*  window = glfwCreateWindow(Window::g_width, Window::g_height, "light", nullptr, nullptr);
    if(!window){
        std::println("failed to initialize window");
        std::terminate();
    } 
    glfwMakeContextCurrent(window);
    glfwWindowHint(GLFW_RESIZABLE, false);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    return window;
}