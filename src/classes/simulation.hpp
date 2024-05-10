#pragma once

#include <vector>
#include <memory>
#include "particle.hpp"
#include "plane.hpp"

class Simulation {
public:
    std::vector<std::shared_ptr<Particle>> particles;
    std::vector<std::shared_ptr<Plane>> planes;

    void step(float dt);  // update simulation by time dt
    void checkCollisions();  // check for collisions between particles and planes
    void addForce(glm::vec3 force);  // add force to all particles
};