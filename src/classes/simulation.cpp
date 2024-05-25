#include "simulation.hpp"
#include <omp.h>
#include <iostream>
#include "container.hpp"
#include "grid.hpp"
#ifndef _OPENMP
    #define _OPENMP 0
#endif



Simulation::Simulation() {
    // cout some info about omp version
    std::cout << "OpenMP version: " << _OPENMP << std::endl; // _OPENMP is defined by g++, not msvc
    int num_threads = omp_get_max_threads();
    std::cout << "Number of OMP threads: " << num_threads << std::endl;
    omp_set_num_threads(num_threads);

    // setup the grid
    grid = std::make_unique<Grid>(0.30f); // here particle radius is 0.15
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
    const int num_particles = static_cast<int>(spheres.size());
    #pragma omp parallel for schedule(static, 1)
    for (int i = 0; i < num_particles; ++i) {
        // * collision with other particles
        for (size_t j = i + 1; j < num_particles; ++j) {
            spheres[i]->collideWith(spheres[j]);
        }
        // * collision with planes
        // for (auto& plane : planes) {
        //     particles[i]->collideWith(plane);
        // }
        // * collision with containers
        for (auto& container : containers) {
            particles[i]->collideWith(container);
        }
    }
}

// ? method 1 : iterate over the particles
// void Simulation::checkGridCollisions() {
//     const int num_particles = (int)spheres.size();
//     // clear the grid
//     grid->clear();
//     for (int i = 0; i < num_particles; ++i) {
//         grid->insert(spheres[i]);
//     }
//         // check for collisions
//         #pragma omp parallel for schedule(dynamic, 10)
//         for (int i = 0; i < num_particles; ++i) {
//             std::vector<std::shared_ptr<Sphere>> neighbors = grid->getNeighbors(spheres[i]);
//             for (auto& neighbor : neighbors) {
//                 spheres[i]->collideWith(neighbor);
//             }
//             for (auto& container : containers) {
//                 particles[i]->collideWith(container);
//             }
//         }
// }

// ? method 2 : iterate over the grid
void Simulation::checkGridCollisions() {
    // clear the grid
    grid->clear();
    for (auto& s: spheres) {
        grid->insert(s);
    }
    std::vector<std::pair<glm::ivec3, std::vector<std::shared_ptr<Sphere>>>> gridAsVector(grid->grid.begin(), grid->grid.end());
    const int num_cells = static_cast<int>(gridAsVector.size());
    #pragma omp parallel for schedule(static, 1)
    for (int i = 0; i < num_cells; ++i) {
        std::vector<std::shared_ptr<Sphere>> neighbors = grid->getNeighbors(gridAsVector[i].first);
        for (auto& s : gridAsVector[i].second) {
            for (auto& neighbor : neighbors) {
                if (s != neighbor) {
                    s->collideWith(neighbor);
                }
            }
            for (auto& container : containers) {
                s->collideWith(container);
            }
        }
    }

    
}

void Simulation::addForce(glm::vec3 force) {
    for (auto& p : particles) {
        p->addForce(force);
    }
}

std::shared_ptr<Sphere> Simulation ::createSphere(glm::vec3 position, float radius, glm::vec3 velocity, glm::vec3 acceleration, bool fixed) {
    auto p = std::make_shared<Sphere>();
    p->position = position;
    p->previous_position = position;
    p->velocity = velocity;
    p->acceleration = acceleration;
    p->radius = radius;
    p->fixed = fixed;
    particles.push_back(p);
    spheres.push_back(p);
    this->num_particles++;
    return p;
}

void Simulation::createCubeContainer(glm::vec3 position, glm::vec3 size, bool fordedInside) {
    auto cc = std::make_shared<CubeContainer>(position, size, fordedInside);
    containers.push_back(cc);
}

void Simulation::maintainMolecules() {
    for (auto& m : molecules) {
        if (m->linksEnabled) {
            m->maintainDistanceLinks();
        } else {
            m->maintainDistanceAll();
        }
    }
}