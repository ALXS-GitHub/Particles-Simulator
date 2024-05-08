#pragma once

#include <vector>
#include <memory>
#include "particle.hpp"

class Simulation {
public:
    std::vector<std::shared_ptr<Particle>> particles;

    void step(float dt);  // update simulation by time dt
};