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