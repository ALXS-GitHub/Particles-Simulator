#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <memory>

class Container {
    public:
        glm::vec3 position;

        Container(glm::vec3 position);
        glm::vec3 getPosition();
        void setPosition(glm::vec3 position);
};

class CubeContainer : public Container {
    public:
        glm::vec3 size; // length, width, height

        CubeContainer(glm::vec3 position, glm::vec3 size);
        glm::vec3 getSize();
        void setSize(glm::vec3 size);
};