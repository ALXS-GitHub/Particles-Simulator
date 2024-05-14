#include "simulation.hpp"
#include <omp.h>
#include <iostream>
#include "container.hpp"
#ifndef _OPENMP
    #define _OPENMP 0
#endif

Simulation::Simulation() {
    // cout some info about omp version
    std::cout << "OpenMP version: " << _OPENMP << std::endl; // _OPENMP is defined by g++, not msvc
    int num_threads = omp_get_max_threads();
    std::cout << "Number of OMP threads: " << num_threads << std::endl;
    omp_set_num_threads(num_threads);
}

int Simulation::getNumParticles() {
    return num_particles;
}

void Simulation::step(float dt) {
    for (auto& p : particles) {
        p->updatePosition(dt);
    }
}

void Simulation::checkCollisions() {
    const int num_particles = static_cast<int>(particles.size());
    #pragma omp parallel for
    for (int i = 0; i < num_particles; ++i) {
        // * collision with other particles
        for (size_t j = i + 1; j < num_particles; ++j) {
            auto sphere = std::dynamic_pointer_cast<Sphere>(particles[j]);
            if (sphere) {
                particles[i]->collideWith(sphere);
            }
        }
        // * collision with planes
        for (auto& plane : planes) {
            particles[i]->collideWith(plane);
        }
        // * collision with containers
        for (auto& container : containers) {
            particles[i]->collideWith(container);
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
    spheres.push_back(p);
    this->num_particles++;
}

void Simulation::createCubeContainer(glm::vec3 position, glm::vec3 size) {
    auto cc = std::make_shared<CubeContainer>(position, size);
    containers.push_back(cc);
}