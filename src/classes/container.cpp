#include "container.hpp"

#include <vector>
#include <glm/glm.hpp>
#include <memory>

Container::Container(glm::vec3 position, bool forcedInside) {
    this->position = position;
    this->forcedInside = forcedInside;
}

glm::vec3 Container::getPosition() {
    return position;
}

void Container::setPosition(glm::vec3 position) {
    this->position = position;
}

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