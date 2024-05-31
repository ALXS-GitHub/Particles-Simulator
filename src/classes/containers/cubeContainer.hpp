#pragma once
#include "../container.hpp"
#include <glm/glm.hpp>

class CubeContainer : public Container {
    public:
        glm::vec3 size; // length, width, height

        CubeContainer(glm::vec3 position, glm::vec3 size, bool forcedInside = false);
        glm::vec3 getSize();
        void setSize(glm::vec3 size);
};