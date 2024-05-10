#include "simulation.hpp"

int Simulation::getNumParticles() {
    return num_particles;
}

void Simulation::step(float dt) {
    for (auto& p : particles) {
        p->updatePosition(dt);
    }
}

void Simulation::checkCollisions() {
    for (auto& p1 : particles) {
        for (auto& p2 : particles) {
            if (p1 != p2) {
                // TODO prevent this dynamic cast later
                auto sphere = std::dynamic_pointer_cast<Sphere>(p2);
                if (sphere) {
                    p1->collideWith(sphere);
                }
            }
        }
        for (auto& plane : planes) {
            p1->collideWith(plane);
        }
    }
}

void Simulation::addForce(glm::vec3 force) {
    for (auto& p : particles) {
        p->addForce(force);
    }
}

void Simulation ::createSphere(glm::vec3 position, float radius, glm::vec3 velocity, glm::vec3 acceleration) {
    auto p = std::make_shared<Sphere>();
    p->position = position;
    p->previous_position = position;
    p->velocity = velocity;
    p->acceleration = acceleration;
    p->radius = radius;
    particles.push_back(p);
    this->num_particles++;
}