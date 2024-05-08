#include "simulation.hpp"

void Simulation::step(float dt) {
    // TODO change this because it is not correct
    for (auto& p : particles) {
        glm::vec3 previous_position = p->position;
        p->position += p->position - p->previous_position + (p->force / p->mass) * dt * dt;
        p->previous_position = previous_position;
    }
}