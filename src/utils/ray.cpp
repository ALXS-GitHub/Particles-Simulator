#include "ray.hpp"
#include <glm/glm.hpp>

Ray::Ray(const glm::vec3& origin, const glm::vec3& direction) : origin(origin), direction(direction) {}

float Ray::intersect(const std::shared_ptr<Sphere>& sphere) {
        glm::vec3 oc = origin - sphere->position;
        float a = glm::dot(direction, direction);
        float b = 2.0f * glm::dot(oc, direction);
        float c = glm::dot(oc, oc) - sphere->radius * sphere->radius;
        float discriminant = b * b - 4 * a * c;

        if (discriminant < 0) {
            return std::numeric_limits<float>::max();
        } else {
            return (-b - sqrt(discriminant)) / (2.0f * a);
        }
    }