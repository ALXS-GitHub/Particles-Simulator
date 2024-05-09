#include "particle.hpp"
#include <glm/glm.hpp>

using namespace glm;

void Particle::addForce(vec3 force) {
    acceleration += force;
}