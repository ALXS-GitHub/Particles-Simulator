#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <memory>

class Container {
    private:
        bool forcedInside; // this is used to force the particles inside the container, mainly if the dt in the simulation is high (or not many substeps), but be careful with this, it can lead to some weird behavior if you have multiple containers

    public:
        glm::vec3 position;

        Container(glm::vec3 position, bool forcedInside = false);
        glm::vec3 getPosition();
        void setPosition(glm::vec3 position);
        bool getForcedInside();
};

class CubeContainer : public Container {
    public:
        glm::vec3 size; // length, width, height

        CubeContainer(glm::vec3 position, glm::vec3 size, bool forcedInside = false);
        glm::vec3 getSize();
        void setSize(glm::vec3 size);
};