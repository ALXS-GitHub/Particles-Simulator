#pragma once

#include <glm/glm.hpp>
#include <memory>
#include "../classes/particle.hpp"

class Ray {
public:
    glm::vec3 origin;
    glm::vec3 direction;

    Ray(const glm::vec3& origin, const glm::vec3& direction);

    float intersect(const std::shared_ptr<Sphere>& sphere);
};