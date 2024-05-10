#pragma once

#include <vector>
#include <memory>
#include "particle.hpp"
#include "plane.hpp"

class Simulation {
private: 
    int num_particles;

public:
    std::vector<std::shared_ptr<Particle>> particles;
    std::vector<std::shared_ptr<Plane>> planes;

    void getNumParticles();
    void step(float dt);  // update simulation by time dt
    void checkCollisions();  // check for collisions between particles and planes
    void addForce(glm::vec3 force);  // add force to all particles
    void createSphere(glm::vec3 position, float radius, glm::vec3 velocity, glm::vec3 acceleration);  // add a sphere to the simulation
};