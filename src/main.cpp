#pragma warning(push, 0)
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#pragma warning(pop)
#include <print>

#include "camera.hpp"
#include "globals.hpp"
#include "shaders.hpp"


Camera camera{glm::vec3(0.0f, 0.0f, 3.0f)};
float lastX{Window::g_width/ 2.0f};
float lastY{Window::g_height/ 2.0f};
bool firstMouse{true};

struct vertexBufferIDs{
    uint32_t VAO{};
    uint32_t VBO{};
    uint32_t EBO{};
};

float deltaTime{0.0f};
float lastFrame{0.0f};

glm::vec3 lightPos{1.2f, 1.0f, 2.0f};

GLFWwindow* setup_GLFW();

void process_input(GLFWwindow*);

int main(){
    auto window{setup_GLFW()};

    if(!gladLoadGLLoader( (GLADloadproc)glfwGetProcAddress )) {
        std::println("failed to initialize glad");
        return -1;
    }
    glEnable(GL_DEPTH_TEST);

    vertexBufferIDs cube;
    glGenVertexArrays(1, &cube.VAO);
    glGenBuffers(1, &cube.VBO);
    glBindVertexArray(cube.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, cube.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(float) * 3, (void*)0);
    glEnableVertexAttribArray(0);

    uint32_t light_VAO; 
    glGenVertexArrays(1, &light_VAO);
    glBindVertexArray(light_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, cube.VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(float) * 3, (void*) 0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    uint32_t cube_vertex_shader{glCreateShader(GL_VERTEX_SHADER)};
    uint32_t cube_fragment_shader{glCreateShader(GL_FRAGMENT_SHADER)};
    uint32_t light_fragment_shader{glCreateShader(GL_FRAGMENT_SHADER)};

    int32_t success{};
    char info_log[512];

    auto check_shader_compile = [&success, &info_log](uint32_t shader){
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if(!success){
            glGetShaderInfoLog(shader, 512, nullptr, info_log);
            std::println("Failed to compile shader: \n{}", info_log);
        }
    };

    glShaderSource(cube_vertex_shader, 1, &cube_vertex_shader_source, nullptr);
    glCompileShader(cube_vertex_shader);
    check_shader_compile(cube_vertex_shader);
    glShaderSource(cube_fragment_shader, 1, &cube_fragment_shader_source, nullptr);
    glCompileShader(cube_fragment_shader);
    check_shader_compile(cube_fragment_shader);
    glShaderSource(light_fragment_shader, 1, &light_fragment_shader_source, nullptr);
    glCompileShader(light_fragment_shader);
    check_shader_compile(light_fragment_shader);

    uint32_t cube_shader_program{glCreateProgram()};
    uint32_t light_shader_program{glCreateProgram()};

    glAttachShader(cube_shader_program, cube_vertex_shader);
    glAttachShader(cube_shader_program, cube_fragment_shader);
    glAttachShader(light_shader_program, cube_vertex_shader);
    glAttachShader(light_shader_program, light_fragment_shader);

    glLinkProgram(cube_shader_program);
    glLinkProgram(light_shader_program);

    auto check_program_link = [&success, &info_log](uint32_t ID){
        glGetProgramiv(ID, GL_LINK_STATUS, &success);
        if(!success){
            glGetProgramInfoLog(ID, 512, nullptr, info_log);
            std::println("Failed to link program: \n{}", info_log);
        }
    };

    check_program_link(cube_shader_program);
    check_program_link(light_shader_program);

    glDeleteShader(cube_vertex_shader);
    glDeleteShader(cube_fragment_shader);
    glDeleteShader(light_fragment_shader);
    
    glUseProgram(cube_shader_program);
    float color1[]{1.0f, 1.0f, 1.0f};
    float color2[]{1.0f, 0.5f, 0.31f};
    glUniform3fv(glGetUniformLocation(cube_shader_program, "objectColor"), 1, color2);
    glUniform3fv(glGetUniformLocation(cube_shader_program, "lightColor"), 1, color1);
    glUseProgram(0);



    while(!glfwWindowShouldClose(window)){
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = lastFrame - currentFrame;
        lastFrame = currentFrame;

        process_input(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glm::mat4 projection{glm::perspective(glm::radians(camera.m_zoom),
        static_cast<float>(Window::g_width)/static_cast<float>(Window::g_height),
        0.1f, 100.0f)};
        glm::mat4 view{camera.getViewMatrix()};
        glm::mat4 model{glm::mat4(1.0f)};

        glUseProgram(cube_shader_program);
        glUniformMatrix4fv(glGetUniformLocation(cube_shader_program, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(cube_shader_program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(cube_shader_program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        
        glBindVertexArray(cube.VAO);
        glDrawArrays(GL_TRIANGLES, 0 ,36);
        
        glUseProgram(light_shader_program);
        glUniformMatrix4fv(glGetUniformLocation(light_shader_program, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(light_shader_program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f));
        glUniformMatrix4fv(glGetUniformLocation(light_shader_program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        
        glBindVertexArray(light_VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
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