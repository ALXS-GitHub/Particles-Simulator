#pragma once

#include <glm/glm.hpp>
#include "plane.hpp"
#include "container.hpp"
#include <memory>


using namespace glm;

// ? forward declarations
class Particle;
class Sphere;
class Container;

class Particle {
    bool updatingEnabled = true;

public:
    vec3 previous_position;
    vec3 position;
    vec3 velocity;
    vec3 acceleration;
    
    bool fixed = false; // whether the particle is fixed in space

    virtual ~Particle() = default;
    virtual void updatePosition(float dt);
    virtual void collideWith(std::shared_ptr<Sphere> sphere) = 0;
    virtual void collideWith(std::shared_ptr<Plane> plane) = 0;
    virtual void collideWith(std::shared_ptr<Container> container) = 0;
    void addForce(vec3 force);
    void move(vec3 move); // move the particle by a certain amount
    void setUpdatingEnabled(bool enabled);
};

// particle classes
#include "particles/sphere.hpp"

// Path: src/classes/particle.hpp