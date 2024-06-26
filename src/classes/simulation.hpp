#pragma once

#include <vector>
#include <memory>
#include "particle.hpp"
#include "plane.hpp"
#include "container.hpp"
#include "grid.hpp"
#include "molecule.hpp"

class Simulation {
private: 
    int num_particles = 0;
    int num_threads = 4;

public:
    std::unique_ptr<Grid> grid; // unique_ptr because only the simulation class should own the grid

    std::vector<std::shared_ptr<Particle>> particles;
    std::vector<std::shared_ptr<Sphere>> spheres;
    std::vector<std::shared_ptr<Plane>> planes;
    std::vector<std::shared_ptr<Container>> containers;
    std::vector<std::shared_ptr<Container>> cubeContainers;
    std::vector<std::shared_ptr<Container>> sphereContainers;
    std::vector<std::shared_ptr<Molecule>> molecules;

    Simulation();

    int getNumParticles();
    void step(float dt);  // update simulation by time dt
    void checkCollisions();  // check for collisions between particles and other elements // old method (doesn't use the grid)
    void checkGridCollisions();  // check for collisions between particles and spheres
    void addForce(glm::vec3 force);  // add force to all particles
    void createCubeContainer(glm::vec3 position, glm::vec3 size, bool fordedInside = false);  // add a cube container to the simulation
    void createSphereContainer(glm::vec3 position, float radius, bool fordedInside = false);  // add a sphere container to the simulation
    void maintainMolecules();  // maintain the distance between the spheres in the molecules
    std::shared_ptr<Sphere> createSphere(std::shared_ptr<Sphere> sphere);  // add a sphere to the simulation
    std::shared_ptr<Sphere> createSphere(glm::vec3 position, float radius, glm::vec3 velocity, glm::vec3 acceleration, bool fixed = false);  // add a sphere to the simulation
    std::shared_ptr<Molecule> loadMolecule(std::string filename, glm::vec3 offset = glm::vec3(0.0f));  // load a molecule from a json file
    void loadWorld(std::string filename);  // load the world from a json file
};