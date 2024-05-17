#include "simulation.hpp"
#include <omp.h>
#include <mpi.h>
#include <iostream>
#include "container.hpp"
#include "grid.hpp"
#include <map>
#include <unordered_map>
#include "../utils/mpi_manager.hpp"

Simulation::Simulation() {
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

void Simulation ::createSphere(glm::vec3 position, float radius, glm::vec3 velocity, glm::vec3 acceleration) {
    auto p = std::make_shared<Sphere>();
    p->position = position;
    p->previous_position = position;
    p->velocity = velocity;
    p->acceleration = acceleration;
    p->radius = radius;
    p->ID = this->num_particles;
    particles.push_back(p);
    spheres.push_back(p);
    this->num_particles++;
}

void Simulation::createCubeContainer(glm::vec3 position, glm::vec3 size, bool fordedInside) {
    auto cc = std::make_shared<CubeContainer>(position, size, fordedInside);
    containers.push_back(cc);
}

// * using MPI
// ideas on how to do :
// first add an id to each particle
// then divide the grid and send those parts to each process (they will only process the particles in their part)
// then send the particles back to the master process that will receive back the particles and update the positions
// keep track of particles that are already received and if so, don't update them again (or other method : take the mean of the positions)
void Simulation::checkGridCollisionsMPI() {
    // clear the grid
    grid->clear();
    for (auto& s: spheres) {
        grid->insert(s);
    }
    std::vector<std::pair<glm::ivec3, std::vector<std::shared_ptr<Sphere>>>> gridAsVector(grid->grid.begin(), grid->grid.end());
    // serialize data into a vector [grid.x, grid.y, grid.z, position.x, position.y, position.z, radius]
    std::vector<float> data;
        int ID = 0;
    for (auto& cell : gridAsVector) {
        for (auto& s : cell.second) {
            data.push_back(s->ID);
            data.push_back(cell.first.x);
            data.push_back(cell.first.y);
            data.push_back(cell.first.z);
            data.push_back(s->position.x);
            data.push_back(s->position.y);
            data.push_back(s->position.z);
            data.push_back(s->radius);
            // ! each particle in the data takes 8 floats
        }
    }
    int data_size = static_cast<int>(data.size());
    MPI_Send(&data_size, 1, MPI_INT, 1, 0, MPI_COMM_WORLD); // ! for the moment, we only send to process 1
    MPI_Send(data.data(), data.size(), MPI_FLOAT, 1, 0, MPI_COMM_WORLD); // ! for the moment, we only send to process 1
    std::vector<float> container_data;
    for (auto& container : containers) {
        container_data.push_back(container->position.x);
        container_data.push_back(container->position.y);
        container_data.push_back(container->position.z);
        container_data.push_back(container->size.x);
        container_data.push_back(container->size.y);
        container_data.push_back(container->size.z);
    }
    // only sending 1 container for the moment
    MPI_Send(container_data.data(), 6, MPI_FLOAT, 1, 0, MPI_COMM_WORLD); // ! for the moment, we only send to process 1

    // make a copy of data
    std::vector<float> data_copy;
    data_copy.insert(data_copy.end(), data.begin(), data.end());

    // process 0 process its own data
    checkGridCollisionsMPIProcess(data, container_data);

    // receive the data from the other process
    std::vector<float> recv_data(data_size);
    MPI_Recv(recv_data.data(), data_size, MPI_FLOAT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    // update the positions of the particles
    std::map <int, glm::vec3> idToSphere;
    

    for (int i = 0; i < data_size / 8; ++i) {
        // take the mean of the current data and received data
        glm::vec3 init_position = glm::vec3(data_copy[i * 8 + 4], data_copy[i * 8 + 5], data_copy[i * 8 + 6]);
        glm::vec3 position = glm::vec3(data[i * 8 + 4], data[i * 8 + 5], data[i * 8 + 6]);
        glm::vec3 recv_position = glm::vec3(recv_data[i * 8 + 4], recv_data[i * 8 + 5], recv_data[i * 8 + 6]);
        
        if (init_position != position) {
            idToSphere[data[i * 8]] = position;
        } else {
            idToSphere[data[i * 8]] = recv_position;
        }
    }

    for (auto& s : spheres) {
        s->position = idToSphere[s->ID];
    }
}

struct compareFunc {
    bool operator()(const glm::ivec3& a, const glm::ivec3& b) const {
        if (a.x != b.x)
            return a.x < b.x;
        if (a.y != b.y)
            return a.y < b.y;
        return a.z < b.z;
    }
};
std::vector<int> getNeighborsID(glm::ivec3& cell, std::map<glm::ivec3, std::vector<int>, compareFunc>& grid) {
    std::vector<int> neighbors;
    for (int i = -1; i <= 1; ++i) {
        for (int j = -1; j <= 1; ++j) {
            for (int k = -1; k <= 1; ++k) {
                if (grid.find(glm::ivec3(cell.x + i, cell.y + j, cell.z + k)) != grid.end()) {
                    neighbors.insert(neighbors.end(), grid[glm::ivec3(cell.x + i, cell.y + j, cell.z + k)].begin(), grid[glm::ivec3(cell.x + i, cell.y + j, cell.z + k)].end());
                }
            }
        }
    }
    return neighbors;
}


// function for each individual process
void Simulation::checkGridCollisionsMPIProcess(std::vector<float>& data, std::vector<float>& container_data) {
    const int num_cells = data.size() / 8;

    int rank = MPIManager::getRank();
    int size = MPIManager::getSize();
    // Calculate the range of cells that this process should handle
    int cells_per_process = num_cells / size;
    int start_cell = rank * cells_per_process;
    int end_cell = (rank == size - 1) ? num_cells : start_cell + cells_per_process;

    // make a grid inside the current process
    std::map<glm::ivec3, std::vector<int>, compareFunc> grid;
    for (int i = 0; i < num_cells; ++i) {
        glm::ivec3 cell = glm::ivec3(data[i * 8 + 1], data[i * 8 + 2], data[i * 8 + 3]);
        grid[cell].push_back(i);
    }

    // check for collisions
    #pragma omp parallel for schedule(static, 1)
    for (int i = start_cell; i < end_cell; ++i) {
        glm::ivec3 cell = glm::ivec3(data[i * 8 + 1], data[i * 8 + 2], data[i * 8 + 3]);
        std::vector<int> neighbors = getNeighborsID(cell, grid);

        for (auto& p1: neighbors) {
            float p1Data[4] = {data[p1 * 8 + 4], data[p1 * 8 + 5], data[p1 * 8 + 6], data[p1 * 8 + 7]};
            for (auto& p2: neighbors) {
                if (p1 != p2) {
                    float p2Data[4] = {data[p2 * 8 + 4], data[p2 * 8 + 5], data[p2 * 8 + 6], data[p2 * 8 + 7]};
                    
                    // check for collision
                    glm::vec3 axis = glm::vec3(p1Data[0], p1Data[1], p1Data[2]) - glm::vec3(p2Data[0], p2Data[1], p2Data[2]); // vector between the two spheres
                    float distance = glm::length(axis); // distance between the two spheres
                    float overlap = p1Data[3] + p2Data[3] - distance; // overlap between the two spheres (if there is one)
                    if (overlap > 0) { // if there is a collision
                        axis = glm::normalize(axis);
                        glm::vec3 move = axis * overlap * 0.5f; // move the spheres by half the overlap
                        data[p1 * 8 + 4] += move.x;
                        data[p1 * 8 + 5] += move.y;
                        data[p1 * 8 + 6] += move.z;
                        data[p2 * 8 + 4] -= move.x;
                        data[p2 * 8 + 5] -= move.y;
                        data[p2 * 8 + 6] -= move.z;
                    }
                }
            }

            // check for collision with containers
            if (p1Data[0] - p1Data[3] < container_data[0] - container_data[3] / 2) {
                data[p1 * 8 + 4] = container_data[0] - container_data[3] / 2 + p1Data[3];
            }
            if (p1Data[0] + p1Data[3] > container_data[0] + container_data[3] / 2) {
                data[p1 * 8 + 4] = container_data[0] + container_data[3] / 2 - p1Data[3];
            }
            if (p1Data[1] - p1Data[3] < container_data[1] - container_data[4] / 2) {
                data[p1 * 8 + 5] = container_data[1] - container_data[4] / 2 + p1Data[3];
            }
            if (p1Data[1] + p1Data[3] > container_data[1] + container_data[4] / 2) {
                data[p1 * 8 + 5] = container_data[1] + container_data[4] / 2 - p1Data[3];
            }
            if (p1Data[2] - p1Data[3] < container_data[2] - container_data[5] / 2) {
                data[p1 * 8 + 6] = container_data[2] - container_data[5] / 2 + p1Data[3];
            }
            if (p1Data[2] + p1Data[3] > container_data[2] + container_data[5] / 2) {
                data[p1 * 8 + 6] = container_data[2] + container_data[5] / 2 - p1Data[3];
            }
        }        
    }
}