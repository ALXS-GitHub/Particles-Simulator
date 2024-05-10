#include "simulation.hpp"

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