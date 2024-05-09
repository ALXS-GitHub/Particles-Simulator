#include "simulation.hpp"

void Simulation::step(float dt) {
    // TODO check if this is the correct way to update the position
    for (auto& p : particles) {
        glm::vec3 previous_position = p->position;
        p->position += p->position - p->previous_position + p->acceleration * dt * dt;
        p->previous_position = previous_position;
        p->velocity = (p->position - p->previous_position) / dt; // for information only
        p->acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
    }
}

void Simulation::addForce(glm::vec3 force) {
    for (auto& p : particles) {
        p->addForce(force);
    }
}