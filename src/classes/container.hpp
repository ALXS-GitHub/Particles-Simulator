#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <memory>
#include "particle.hpp"

// ? forward declaration
class Sphere;

class Container {
    protected:
        bool forcedInside; // this is used to force the particles inside the container, mainly if the dt in the simulation is high (or not many substeps), but be careful with this, it can lead to some weird behavior if you have multiple containers

    public:
        glm::vec3 position;
        glm::vec3 size;

        Container(glm::vec3 position, bool forcedInside = false);
        glm::vec3 getPosition();
        void setPosition(glm::vec3 position);
        bool getForcedInside();

        virtual void collideWith(Sphere* sphere) = 0;
};

// container classes
#include "./containers/cubeContainer.hpp"
#include "./containers/sphereContainer.hpp"