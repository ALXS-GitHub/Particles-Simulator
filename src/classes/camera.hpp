#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 up;

    Camera(glm::vec3 position, glm::vec3 direction, glm::vec3 up)
        : position(position), direction(direction), up(up) {}

    glm::mat4 getViewMatrix() const {
        return glm::lookAt(position, position + direction, up);
    }

    glm::mat4 getProjectionMatrix(float fov, float aspect, float near, float far) const {
        return glm::perspective(fov, aspect, near, far);
    }
};