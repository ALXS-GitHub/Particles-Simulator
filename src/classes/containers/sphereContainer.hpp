#pragma once
#include "../container.hpp"
#include <glm/glm.hpp>

class SphereContainer : public Container {
    public:

        SphereContainer(glm::vec3 position, glm::vec3 size, bool forcedInside = false);
        glm::vec3 getSize();
        void setSize(glm::vec3 size);

        void collideWith(Sphere* sphere) override;
};