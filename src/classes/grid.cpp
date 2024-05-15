#include "grid.hpp"
#include "simulation.hpp"
#include "particle.hpp"
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <iostream>
#include <map>
#include <unordered_map>



Grid::Grid(float cellSize) {
    this->cellSize = cellSize;
}

glm::ivec3 Grid::getCell(glm::vec3 position) {
    return glm::ivec3(position / cellSize); // converting to the largest integer less than or equal to the value
}

void Grid::insert(std::shared_ptr<Sphere> sphere) {
    glm::ivec3 cell = getCell(sphere->position);
    grid[cell].push_back(sphere);
}

void Grid::clear() {
    grid.clear();
}

std::vector<std::shared_ptr<Sphere>> Grid::getNeighbors(std::shared_ptr<Sphere> sphere) {
    std::vector<std::shared_ptr<Sphere>> neighbors;
    glm::ivec3 cell = getCell(sphere->position);
    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            for (int z = -1; z <= 1; ++z) {
                glm::ivec3 neighbor_cell = cell + glm::ivec3(x, y, z);
                std::vector<std::shared_ptr<Sphere>> temp;
                if (grid.find(neighbor_cell) != grid.end()) {
                    temp = grid[neighbor_cell];
                } else {
                    continue;
                }
                for (auto& s : temp) {
                    if (s != sphere) {
                        neighbors.push_back(s);
                    }
                }
            }
        }
    }
    return neighbors;
}

std::vector<std::shared_ptr<Sphere>> Grid::getNeighbors(glm::ivec3 cell) {
    std::vector<std::shared_ptr<Sphere>> neighbors;
    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            for (int z = -1; z <= 1; ++z) {
                glm::ivec3 neighbor_cell = cell + glm::ivec3(x, y, z);
                std::vector<std::shared_ptr<Sphere>> temp;
                if (grid.find(neighbor_cell) != grid.end()) {
                    temp = grid[neighbor_cell];
                } else {
                    continue;
                }
                for (auto& s : temp) {
                    neighbors.push_back(s);
                }
            }
        }
    }
    return neighbors;
}