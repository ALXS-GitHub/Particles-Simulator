#pragma once

#include <glm/glm.hpp>
#include "plane.hpp"
#include "container.hpp"
#include <memory>

using namespace glm;

class Particle;
class Sphere;

class Particle {
public:
    int ID;
    vec3 previous_position;
    vec3 position;
    vec3 velocity;
    vec3 acceleration;
    // other properties...

    virtual ~Particle() = default;
    virtual void updatePosition(float dt);
    virtual void collideWith(std::shared_ptr<Sphere> sphere) = 0;
    virtual void collideWith(std::shared_ptr<Plane> plane) = 0;
    virtual void collideWith(std::shared_ptr<CubeContainer> container) = 0;
    void addForce(vec3 force);
};

class Sphere : public Particle {
public:
    float radius;
    // other properties...
    void collideWith(std::shared_ptr<Sphere> sphere) override;
    void collideWith(std::shared_ptr<Plane> plane) override;
    void collideWith(std::shared_ptr<CubeContainer> container) override;
};

// Path: src/classes/particle.hpp