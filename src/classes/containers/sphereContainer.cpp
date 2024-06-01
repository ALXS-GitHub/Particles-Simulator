#include "sphereContainer.hpp"
#include <glm/glm.hpp>

SphereContainer::SphereContainer(glm::vec3 position, glm::vec3 size, bool forcedInside) : Container(position, forcedInside) {
    this->size = size;
}

glm::vec3 SphereContainer::getSize() {
    return size;
}

void SphereContainer::setSize(glm::vec3 size) {
    this->size = size;
}

void SphereContainer::collideWith(Sphere* sphere) {
    // Collision resolution code
    glm::vec3 spherePosition = sphere->position;
    float sphereRadius = sphere->radius;
    float radius = size.x;
    // Calculate the distance between the sphere's position and the container's center
    glm::vec3 axis = spherePosition - position;
    float distance = glm::length(axis);
    
    // Check if the sphere is inside the container
    if (!forcedInside && distance > radius) {
        // the sphere is outside the container
        return;
    }

    // Collision resolution code
    if (distance + sphereRadius > radius) {
        // Calculate the penetration depth
        float penetration = distance + sphereRadius - radius;
        // Calculate the new position of the sphere
        sphere->position -= penetration * glm::normalize(axis);
    }
}