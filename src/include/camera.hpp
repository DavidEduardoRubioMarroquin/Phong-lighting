#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Defaults{
    constexpr float YAW{-90.0f};
    constexpr float PITCH{0.0f};
    constexpr float SPEED{2.5f};
    constexpr float SENSITIVITY{0.10f};
    constexpr float ZOOM{45.0f};
    constexpr glm::vec3 FRONT{glm::vec3(0.0f, 0.0f, -1.0f)};
};

enum class Camera_Movement{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    MAX_MOVE
};

class Camera{
public:
    //camera properties
    glm::vec3 m_cameraPos{};
    glm::vec3 m_cameraFront{};
    glm::vec3 m_cameraUp{};
    glm::vec3 m_worldUp{};
    glm::vec3 m_cameraRight{};
    //euler angles
    float m_yaw{Defaults::YAW};
    float m_pitch{Defaults::PITCH};
    //configs
    float m_sensitivity{Defaults::SENSITIVITY};
    float m_speed{Defaults::SPEED};
    float m_zoom{Defaults::ZOOM};

    Camera(
        glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
        float yaw = Defaults::YAW,
        float pitch = Defaults::PITCH)
        : m_cameraPos{cameraPos}
        , m_cameraFront{Defaults::FRONT}
        , m_worldUp{up}
        , m_yaw{yaw}
        , m_pitch{pitch}
    {
        updateCameraVectors();
    }

    [[maybe_unused]]explicit Camera(
        float posX = 0.0f,
        float posY = 0.0f,
        float posZ = 0.0f,
        float upX = 0.0f,
        float upY = 1.0f,
        float upZ = 0.0f,
        float yaw = Defaults::YAW,
        float pitch = Defaults::PITCH)
        : m_cameraPos{glm::vec3(posX, posY, posZ)}
        , m_cameraFront{Defaults::FRONT}
        , m_worldUp{glm::vec3(upX, upY, upZ)}
        , m_yaw{yaw}
        , m_pitch{pitch}
    {
        updateCameraVectors();
    }

    glm::mat4 getViewMatrix(){
        return glm::lookAt(m_cameraPos, m_cameraPos + m_cameraFront, m_worldUp);
    }

    void keyInput(Camera_Movement move, float dTime){
        using enum Camera_Movement;
        float velocity{m_speed * dTime};
        switch(move){
            case FORWARD:
                m_cameraPos -= m_cameraFront * velocity;
                break;
            case BACKWARD:
                m_cameraPos += m_cameraFront * velocity;
                break;
            case LEFT:
                m_cameraPos += m_cameraRight * velocity;
                break;
            case RIGHT:
                m_cameraPos -= m_cameraRight * velocity;
                break;
            case MAX_MOVE: break;
        }
    }

    void mouseInput(float xOffset, float yOffset, bool constrainPitch = true){
        xOffset *= m_sensitivity;
        yOffset *= m_sensitivity;

        m_yaw += xOffset;
        m_pitch += yOffset;

        if(constrainPitch){
            if(m_pitch > 89.0f) m_pitch = 89.0f;
            if(m_pitch < -89.0f) m_pitch = -89.0f;
        }

        updateCameraVectors();
    }

    void scrollInput(float yOffset){
        m_zoom -= yOffset;
        if(m_zoom > Defaults::ZOOM) m_zoom = Defaults::ZOOM;
        if(m_zoom < 1.0f) m_zoom = 1.0f;
    }

private:
    void updateCameraVectors(){
        glm::vec3 front{};
        front.x = cosf(glm::radians(m_yaw)) * cosf(glm::radians(m_pitch));
        front.y = sinf(glm::radians(m_pitch));
        front.z = sinf(glm::radians(m_yaw)) * cosf(glm::radians(m_pitch));
        m_cameraFront = glm::normalize(front);

        m_cameraRight = glm::normalize(glm::cross(m_cameraFront, m_worldUp));
        m_cameraUp = glm::normalize(glm::cross(m_cameraRight, m_cameraFront));
    }

};



#endif