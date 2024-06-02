#pragma once

#include <glm/glm.hpp>
#include "../particle.hpp"
#include <stdio.h>

class Sphere : public Particle {
public:
    float radius;
    // other properties...
    Sphere() = default;
    Sphere(glm::vec3 position, float radius, glm::vec3 velocity, glm::vec3 acceleration, bool fixed);
    void collideWith(std::shared_ptr<Sphere> sphere) override;
    void collideWith(std::shared_ptr<Plane> plane) override;
    void collideWith(std::shared_ptr<Container> container) override;
};
