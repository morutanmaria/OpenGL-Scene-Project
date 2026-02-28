#include "Camera.hpp"
#include <glm/gtx/rotate_vector.hpp>

namespace gps {
    //Camera movement taken from lab5
    // Constructor
    Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp)
    {
        this->cameraPosition = cameraPosition;
        this->cameraTarget = cameraTarget;

        cameraFrontDirection = glm::normalize(cameraTarget - cameraPosition);
        cameraRightDirection = glm::normalize(glm::cross(cameraFrontDirection, cameraUp));
        cameraUpDirection = glm::normalize(glm::cross(cameraRightDirection, cameraFrontDirection));
    }

    // Return view matrix
    glm::mat4 Camera::getViewMatrix()
    {
        return glm::lookAt(cameraPosition, cameraPosition + cameraFrontDirection, cameraUpDirection);
    }

    // Move camera
    void Camera::move(MOVE_DIRECTION direction, float speed)
    {
        switch (direction) {
        case MOVE_FORWARD: {
            glm::vec3 flatFront = cameraFrontDirection;
            flatFront.y = 0.0f;                    
            flatFront = glm::normalize(flatFront);
            cameraPosition += speed * flatFront;
            break;
        }
        case MOVE_BACKWARD: {
            glm::vec3 flatFront = cameraFrontDirection;
            flatFront.y = 0.0f;
            flatFront = glm::normalize(flatFront);
            cameraPosition -= speed * flatFront;
            break;
        }
        case MOVE_RIGHT: {
            glm::vec3 flatRight = cameraRightDirection;
            flatRight.y = 0.0f;
            flatRight = glm::normalize(flatRight);
            cameraPosition += speed * flatRight;
            break;
        }
        case MOVE_LEFT: {
            glm::vec3 flatRight = cameraRightDirection;
            flatRight.y = 0.0f;
            flatRight = glm::normalize(flatRight);
            cameraPosition -= speed * flatRight;
            break;
        }
        case MOVE_UP:
            cameraPosition += speed * cameraUpDirection;
            break;
        case MOVE_DOWN:
            cameraPosition -= speed * cameraUpDirection;
            break;
        }

        cameraTarget = cameraPosition + cameraFrontDirection;
    }


    // Rotate camera (pitch = up/down, yaw = left/right)
    void Camera::rotate(float pitch, float yaw)
    {
        glm::vec3 direction;
        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraFrontDirection = glm::normalize(direction);

        cameraRightDirection = glm::normalize(glm::cross(cameraFrontDirection, glm::vec3(0.0f, 1.0f, 0.0f)));
        cameraUpDirection = glm::normalize(glm::cross(cameraRightDirection, cameraFrontDirection));
    }
}
