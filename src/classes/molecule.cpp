#include "molecule.hpp"
#include<glm/glm.hpp>
#include "particle.hpp"
#include <vector>
#include <memory>

Molecule::Molecule(float distance, bool linksEnabled, float strength, float internalPressure, bool useInternalPressure) {
    this->distance = distance;
    this->linksEnabled = linksEnabled;
    this->strength = strength;
    this->internalPressure = internalPressure;
    this->useInternalPressure = useInternalPressure;
}

void Molecule::addSphere(std::shared_ptr<Sphere> sphere) {
    spheres.push_back(sphere);
}

void Molecule::addLink(std::shared_ptr<Sphere> sphere1, std::shared_ptr<Sphere> sphere2) {
    links.push_back(std::make_pair(sphere1, sphere2));
}

void Molecule::maintainDistanceAll() {
    for (int i = 0; i < spheres.size(); i++) {
        for (int j = i + 1; j < spheres.size(); j++) {
            if (i != j)
            maintainDistance(spheres[i], spheres[j]);
        }
    }
}

void Molecule::maintainDistanceLinks() {
    for (auto& link : links) {
        maintainDistance(link.first, link.second);
    }
}

void Molecule::maintainDistance(std::shared_ptr<Sphere> sphere1, std::shared_ptr<Sphere> sphere2) {

    glm::vec3 axis = sphere1->position - sphere2->position; // vector between the two spheres
    float currentDistance = glm::length(axis); // current distance between the two spheres

    // Calculate the correction ratio
    float correctionDistance = (currentDistance - this->distance) / currentDistance * this->strength;

    // Calculate the correction vector
    glm::vec3 correctionVector = axis * correctionDistance;

    // Apply the correction
    sphere1->position -= correctionVector;
    sphere2->position += correctionVector;

    // * see the attractive version bellow
    // glm::vec3 axis = sphere1->position - sphere2->position; // vector between the two spheres
    // float distance = glm::length(axis); // distance between the two spheres
    // float displacement = this->distance - distance; // displacement from the equilibrium position

    // // Hooke's law: F = -k * x
    // float springConstant = 500.0f; // adjust this value to change the stiffness of the spring
    // glm::vec3 force = springConstant * displacement * glm::normalize(axis);

    // sphere1->addForce(force);
    // sphere2->addForce(-force);
}

void Molecule::addInternalPressure() {
    // Calculate the center of the molecule
    glm::vec3 center = glm::vec3(0.0f);
    for (auto& sphere : spheres) {
        center += sphere->position;
    }
    center /= spheres.size();

    for (auto& sphere : spheres) {
        glm::vec3 axis = sphere->position - center; // vector from the center to the sphere
        float currentDistance = glm::length(axis); // current distance from the center

        // Calculate the repulsion force
        float repulsionForce = this->internalPressure * currentDistance;

        // Calculate the correction vector
        glm::vec3 correctionVector = glm::normalize(axis) * repulsionForce;

        // Apply the correction
        sphere->position += correctionVector;
    }
}
