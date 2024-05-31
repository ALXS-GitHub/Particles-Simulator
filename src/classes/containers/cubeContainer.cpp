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