#include "particle.hpp"
#include <glm/glm.hpp>
#include <memory>
#include <iostream>
#include "plane.hpp"
#include "container.hpp"

using namespace std;
using namespace glm;

void Particle::addForce(vec3 force) {
    acceleration += force;
}

void Particle::updatePosition(float dt) {
    if (!fixed) {
        vec3 position_copy = position;
        if (updatingEnabled) 
            position += position - previous_position + acceleration * dt * dt;
        previous_position = position_copy;
        velocity = (position - previous_position) / dt; // for information only
        acceleration = vec3(0.0f, 0.0f, 0.0f);
    }
}

void Particle::move(vec3 move) {
    if (!fixed) {
        position += move;
    }
}

void Particle::setUpdatingEnabled(bool enabled) {
    updatingEnabled = enabled;
}