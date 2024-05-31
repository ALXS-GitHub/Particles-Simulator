#include "simulation.hpp"
#include <omp.h>
#include <iostream>
#include "container.hpp"
#include "grid.hpp"
#include "particle.hpp"
#include "molecule.hpp"
#include <fstream>
#include <json.hpp>
#include <memory>
#include <glm/glm.hpp>
#ifndef _OPENMP
    #define _OPENMP 0
#endif

using json = nlohmann::json;

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

void Simulation::checkCollisions() { // regular collision check without grid
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
        if (m->useInternalPressure) {
            m->addInternalPressure(); // TODO add a parameter to enable or disable this
        }
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

std::shared_ptr<Molecule> Simulation::loadMolecule(std::string filename, glm::vec3 offset) {
    std::ifstream file(filename);
    json j;
    file >> j;

    // Create a new molecule
    float internalPressure = 0.001f;
    bool useInternalPressure = false;
    if (j.find("internalPressure") != j.end()) {
        internalPressure = j["internalPressure"];
        useInternalPressure = true;
    }
    std::shared_ptr<Molecule> molecule = std::make_shared<Molecule>(j["distance"], j["linksEnabled"], j["strength"], internalPressure, useInternalPressure);

    // Create a vector to store the spheres
    std::vector<std::shared_ptr<Sphere>> spheres;

    // Iterate over the spheres in the molecule
    for (const auto& jSphere : j["spheres"]) {

        // init the parameters
        glm::vec3 position;
        float radius;
        glm::vec3 velocity = glm::vec3(0.0f);
        glm::vec3 acceleration = glm::vec3(0.0f);
        bool fixed = false;

        // check for optional parameters
        if (jSphere.find("fixed") != jSphere.end()) {
            fixed = jSphere["fixed"];
        }

        if (jSphere.find("velocity") != jSphere.end()) {
            velocity = glm::vec3(jSphere["velocity"][0], jSphere["velocity"][1], jSphere["velocity"][2]);
        }

        if (jSphere.find("acceleration") != jSphere.end()) {
            acceleration = glm::vec3(jSphere["acceleration"][0], jSphere["acceleration"][1], jSphere["acceleration"][2]);
        }

        // getting required parameters
        position = glm::vec3(jSphere["position"][0], jSphere["position"][1], jSphere["position"][2]);
        position += offset;
        radius = jSphere["radius"];

        // Create a new sphere
        std::shared_ptr<Sphere> sphere = this->createSphere(
            position,
            radius,
            velocity,
            acceleration,
            fixed
        );

        // Add the sphere to the molecule and the spheres vector
        molecule->addSphere(sphere);
        spheres.push_back(sphere);
    }

    // Iterate over the links in the molecule
    for (const auto& jLink : j["links"]) {
        // Add a link between the spheres
        molecule->addLink(spheres[jLink[0]], spheres[jLink[1]]);
    }

    // Add the molecule to the simulation
    this->molecules.push_back(molecule);

    return molecule;
}
