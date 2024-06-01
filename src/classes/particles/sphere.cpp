#include "sphere.hpp"
#include <glm/glm.hpp>

void Sphere::collideWith(std::shared_ptr<Sphere> sphere) {
    glm::vec3 axis = position - sphere->position; // vector between the two spheres
    float distance = glm::length(axis); // distance between the two spheres
    float overlap = radius + sphere->radius - distance; // overlap between the two spheres (if there is one)
    if (overlap > 0) { // if there is a collision
        axis = glm::normalize(axis);
        glm::vec3 move = axis * overlap * 0.5f; // move the spheres by half the overlap
        this->move(move);
        sphere->move(-move);
    }
}

void Sphere::collideWith(std::shared_ptr<Plane> plane) {
    // Collision resolution code
    glm::vec3 normal = plane->getNormal();
    glm::vec2 size = plane->getSize();
    glm::vec3 u;
    // Create two perpendicular vectors on the plane
    if (glm::abs(glm::dot(normal, glm::vec3(1.0f, 0.0f, 0.0f))) < 0.0001f) {
        u = glm::normalize(glm::cross(normal, glm::vec3(1.0f, 0.0f, 0.0f)));
    } else {
        u = glm::normalize(glm::cross(normal, glm::vec3(0.0f, 1.0f, 0.0f)));
    }
        // Scale the vectors by the size of the plane
    glm::vec3 v = glm::cross(normal, u);
    // Project the sphere's position onto the plane
    glm::vec3 projected_position = position - glm::dot(position - plane->getPosition(), normal) * normal;
    // Calculate the distance between the sphere's position and the plane
    glm::vec3 axis = position - projected_position;
    float distance = glm::length(axis);
    // Calculate the distance between the plane's center and the sphere's projected position
    glm::vec3 diff = projected_position - plane->getPosition();
    float u_diff = glm::dot(diff, u);
    float v_diff = glm::dot(diff, v);
    // Calculate the penetration depth
    float penetration = radius - distance;
    if (glm::abs(u_diff) <= size.x / 2 && glm::abs(v_diff) <= size.y / 2) { // check if the sphere is within the plane
        if (penetration > 0) { // if there is a collision
            // Calculate the new position of the sphere
            position += penetration * normal * sign(glm::dot(normal, axis)); // * sign(glm::dot(normal, axis)) to make sure the sphere moves in the right direction
        }
    }
}

void Sphere::collideWith(std::shared_ptr<Container> container) {
    container->collideWith(this);
}