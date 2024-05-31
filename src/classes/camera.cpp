#include <glew.h>
#include "camera.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../config.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <cmath>

Camera::Camera(glm::vec3 position, glm::vec3 direction, glm::vec3 up)
    : position(position), direction(direction), up(up) {}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(position, position + direction, up);
}

glm::mat4 Camera::getProjectionMatrix(float fov, float aspect, float near, float far) const {
    return glm::perspective(fov, aspect, near, far);
}

void Camera::loadMatricesIntoShader(GLuint shaderProgram) const {
    glm::mat4 viewMatrix = getViewMatrix();
    glm::mat4 projectionMatrix = getProjectionMatrix(CAMERA_FOV, CAMERA_ASPECT_RATIO, CAMERA_NEAR, CAMERA_FAR);

    GLint viewMatrixLocation = glGetUniformLocation(shaderProgram, "viewMatrix");
    glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    GLint projectionMatrixLocation = glGetUniformLocation(shaderProgram, "projectionMatrix");
    glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
}

void Camera::moveForward(float distance) {
    // get the direction on the horizontal plane
    glm::vec3 horizontalDirection = direction;
    horizontalDirection.y = 0; // Project onto horizontal plane
    horizontalDirection = glm::normalize(horizontalDirection); // Normalize to get direction on horizontal plane

    position += horizontalDirection * distance;
}

void Camera::moveInDirection(float distance) {
    position += glm::normalize(direction) * distance;
}

void Camera::moveRight(float distance) {
    glm::vec3 horizontalDirection = direction;
    horizontalDirection.y = 0; // Project onto horizontal plane
    horizontalDirection = glm::normalize(horizontalDirection); // Normalize to get direction on horizontal plane

    glm::vec3 right = glm::cross(horizontalDirection, up);
    right = glm::normalize(right);
    position += right * distance;
}

void Camera::moveUp(float distance) {
    position += up * distance;
}

void Camera::moveY(float distance) {
    position.y += distance;
}

// not used for the moment (cause yaw and pitch can lead to some weird angles)
void Camera::rotate(float yaw, float pitch) {
    direction = glm::normalize(glm::rotate(glm::mat4(1.0f), glm::radians(yaw), up) * glm::vec4(direction, 0.0f));
    glm::vec3 right = glm::normalize(glm::cross(direction, up));
    direction = glm::normalize(glm::rotate(glm::mat4(1.0f), glm::radians(pitch), right) * glm::vec4(direction, 0.0f));
    up = glm::normalize(glm::cross(right, direction));
}

void Camera::rotateHorizontal(float angle) {
    glm::vec3 horizontalDirection = direction;
    horizontalDirection.y = 0; // Project onto horizontal plane
    horizontalDirection = glm::normalize(horizontalDirection); // Normalize to get direction on horizontal plane

    // Rotate around the 'y' axis
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    direction = glm::vec3(rotation * glm::vec4(direction, 0.0f));
    up = glm::vec3(rotation * glm::vec4(up, 0.0f));

    // Keep the camera's 'up' vector pointing up
    glm::vec3 right = glm::cross(direction, glm::vec3(0.0f, 1.0f, 0.0f));
    up = glm::cross(right, direction);
}

void Camera::rotateVertical(float angle) {
    // Get the 'right' vector
    glm::vec3 right = glm::normalize(glm::cross(up, direction));

    // Rotate around the 'right' vector
    glm::vec3 newDirection = glm::normalize(glm::rotate(glm::mat4(1.0f), glm::radians(angle), right) * glm::vec4(direction, 0.0f));

    // Check if the new direction is too close to being straight up or straight down
    if (std::abs(glm::dot(newDirection, up)) < 0.95f) {
        direction = newDirection;
    }
}

void Camera::resetYaw() {
    up = glm::vec3(0.0f, 1.0f, 0.0f);
}
