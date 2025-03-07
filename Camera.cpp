#include "Camera.hpp"
#include <glm/gtx/euler_angles.hpp>

namespace gps {

    //Camera constructor
    Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp) {
        this->cameraPosition = cameraPosition;
        this->cameraTarget = cameraTarget;
        this->cameraUpDirection = cameraUp;

        //TODO - Update the rest of camera parameters
        this->cameraFrontDirection = glm::normalize(cameraTarget - cameraPosition);
        this->cameraRightDirection = -glm::normalize(glm::cross(cameraUp, cameraFrontDirection));
        this->cameraOrigUp = cameraUp;
        this->cameraOrigFront = cameraFrontDirection;
    }

    //return the view matrix, using the glm::lookAt() function
    glm::mat4 Camera::getViewMatrix() {
        return glm::lookAt(cameraPosition, cameraTarget, cameraUpDirection);
    }

    //update the camera internal parameters following a camera move event
    void Camera::move(MOVE_DIRECTION direction, float speed) {
        //TODO
        if (direction == MOVE_FORWARD) {
            cameraPosition = cameraPosition + speed * cameraFrontDirection;
        }

        if (direction == MOVE_BACKWARD) {
            cameraPosition = cameraPosition - speed * cameraFrontDirection;
        }

        if (direction == MOVE_RIGHT) {
            cameraPosition = cameraPosition + speed * cameraRightDirection;
        }

        if (direction == MOVE_LEFT) {
            cameraPosition = cameraPosition - speed * cameraRightDirection;
        }

        cameraTarget = cameraPosition + cameraFrontDirection;
    }

    //update the camera internal parameters following a camera rotate event
    //yaw - camera rotation around the y axis
    //pitch - camera rotation around the x axis
    void Camera::rotate(float pitch, float yaw) {
        //TODO
        glm::mat4 eulerAngle = glm::yawPitchRoll(glm::radians(yaw), glm::radians(pitch), 0.0f);

        cameraFrontDirection = glm::vec3(glm::normalize((eulerAngle * glm::vec4(cameraOrigFront, 0.0f))));
        cameraRightDirection = glm::normalize(glm::cross(cameraFrontDirection, cameraOrigUp));
        cameraUpDirection = glm::cross(cameraRightDirection, cameraFrontDirection);

        cameraTarget = cameraPosition + cameraFrontDirection;
    }

    void Camera::animationOfCamera(float angle) {
        // set the camera
        this->cameraPosition = glm::vec3(10, -34, 14);

        // rotate with specific angle around Y axis
        glm::mat4 r = -glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));

        // compute the new position of the camera 
        // previous position * rotation matrix
        this->cameraPosition = glm::vec4(r * glm::vec4(this->cameraPosition, 1));
        this->cameraFrontDirection = glm::normalize(cameraTarget - cameraPosition);
        cameraRightDirection = glm::normalize(glm::cross(cameraFrontDirection, glm::vec3(0.0f, 1.0f, 0.0f)));
    }

}