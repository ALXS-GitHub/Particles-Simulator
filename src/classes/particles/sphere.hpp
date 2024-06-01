#pragma once

#include <glm/glm.hpp>
#include "../particle.hpp"

class Sphere : public Particle {
public:
    float radius;
    // other properties...
    void collideWith(std::shared_ptr<Sphere> sphere) override;
    void collideWith(std::shared_ptr<Plane> plane) override;
    void collideWith(std::shared_ptr<Container> container) override;
};
