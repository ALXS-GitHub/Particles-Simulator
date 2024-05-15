#pragma once

#include "particle.hpp"
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <iostream>
#include <map>
#include <unordered_map>

struct IVec3Hash { // using a FNV-1a hash function
    std::size_t operator()(const glm::ivec3& vec) const {
        std::size_t h = 2166136261u;
        h = (h ^ vec.x) * 16777619u;
        h = (h ^ vec.y) * 16777619u;
        h = (h ^ vec.z) * 16777619u;
        return h;
    }
};


class Grid {

private:
    float cellSize;

public:
    std::unordered_map<glm::ivec3, std::vector<std::shared_ptr<Sphere>>, IVec3Hash> grid;

    Grid(float cellSize);
    glm::ivec3 getCell(glm::vec3 position);
    void insert(std::shared_ptr<Sphere> sphere);
    void clear();
    std::vector<std::shared_ptr<Sphere>> getNeighbors(std::shared_ptr<Sphere> sphere);
    std::vector<std::shared_ptr<Sphere>> getNeighbors(glm::ivec3 cell);
};