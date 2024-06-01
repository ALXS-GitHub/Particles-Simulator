#include "cubeContainer.hpp"
#include <glm/glm.hpp>

CubeContainer::CubeContainer(glm::vec3 position, glm::vec3 size, bool forcedInside) : Container(position, forcedInside) {
    this->size = size;
}

glm::vec3 CubeContainer::getSize() {
    return size;
}

void CubeContainer::setSize(glm::vec3 size) {
    this->size = size;
}

bool Container::getForcedInside() {
    return forcedInside;
}

void CubeContainer::collideWith(Sphere* sphere) {
    // Collision resolution code
    glm::vec3 spherePosition = sphere->position;
    float sphereRadius = sphere->radius;
    glm::vec3 min = position - size / 2.0f; // getting the minimum points of the container
    glm::vec3 max = position + size / 2.0f; // getting the maximum points of the container
    // Check if the sphere is inside the container
    if (!forcedInside && !(min.x <= spherePosition.x && spherePosition.x <= max.x && min.y <= spherePosition.y && spherePosition.y <= max.y && min.z <= spherePosition.z && spherePosition.z <= max.z)) {
        // the sphere is outside the container
        return;
    }

    // Collision resolution code
    if (spherePosition.x - sphereRadius < min.x) {
        sphere->position.x = min.x + sphereRadius;
    } 
    if (spherePosition.x + sphereRadius > max.x) {
        sphere->position.x = max.x - sphereRadius;
    }
    if (spherePosition.y - sphereRadius < min.y) {
        sphere->position.y = min.y + sphereRadius;
    }
    if (spherePosition.y + sphereRadius > max.y) {
        sphere->position.y = max.y - sphereRadius;
    }
    if (spherePosition.z - sphereRadius < min.z) {
        sphere->position.z = min.z + sphereRadius;
    }
    if (spherePosition.z + sphereRadius > max.z) {
        sphere->position.z = max.z - sphereRadius;
    }
}