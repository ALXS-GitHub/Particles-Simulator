#pragma once

#include <glm/glm.hpp>

using namespace glm;

class Particle {
public:
    vec3 previous_position;
    vec3 position;
    vec3 velocity;
    vec3 force;
    float mass;
    // other properties...

    virtual ~Particle() = default;
};

class Sphere : public Particle {
public:
    float radius;
    // other properties...
};

// Path: src/classes/particle.hpp