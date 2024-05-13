#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 up;

    Camera(glm::vec3 position, glm::vec3 direction, glm::vec3 up);

    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix(float fov, float aspect, float near, float far) const;
    
    void moveForward(float distance);
    void moveInDirection(float distance);
    void moveRight(float distance);
    void moveUp(float distance);
    void moveY(float distance);
    void rotate(float yaw, float pitch);
    void rotateHorizontal(float angle);
    void rotateVertical(float angle);
    void resetYaw();

};