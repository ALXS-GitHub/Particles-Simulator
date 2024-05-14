#include "container.hpp"

#include <vector>
#include <glm/glm.hpp>
#include <memory>

Container::Container(glm::vec3 position) {
    this->position = position;
}

glm::vec3 Container::getPosition() {
    return position;
}

void Container::setPosition(glm::vec3 position) {
    this->position = position;
}

CubeContainer::CubeContainer(glm::vec3 position, glm::vec3 size) : Container(position) {
    this->size = size;
}

glm::vec3 CubeContainer::getSize() {
    return size;
}

void CubeContainer::setSize(glm::vec3 size) {
    this->size = size;
}