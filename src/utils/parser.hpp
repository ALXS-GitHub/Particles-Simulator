#pragma once

#include <iostream>
#include "../classes/container.hpp"
#include "../classes/grid.hpp"
#include "../classes/particle.hpp"
#include "../classes/molecule.hpp"
#include <fstream>
#include <json.hpp>
#include <memory>
#include <vector>
#include <glm/glm.hpp>

using json = nlohmann::json;

std::shared_ptr<Sphere> parseSphere(json j);  // parse a sphere from a json object
std::shared_ptr<Molecule> parseMolecule(json j);  // parse a molecule from a json object
std::shared_ptr<Container> parseContainer(json j);  // parse a container from a json object

std::shared_ptr<Sphere> parseSphere(json j) {
    // init the parameters
        glm::vec3 position;
        float radius;
        glm::vec3 velocity = glm::vec3(0.0f);
        glm::vec3 acceleration = glm::vec3(0.0f);
        bool fixed = false;

        // check for optional parameters
        if (j.find("fixed") != j.end()) {
            fixed = j["fixed"];
        }

        if (j.find("velocity") != j.end()) {
            velocity = glm::vec3(j["velocity"][0], j["velocity"][1], j["velocity"][2]);
        }

        if (j.find("acceleration") != j.end()) {
            acceleration = glm::vec3(j["acceleration"][0], j["acceleration"][1], j["acceleration"][2]);
        }

        // getting required parameters
        position = glm::vec3(j["position"][0], j["position"][1], j["position"][2]);
        radius = j["radius"];

        // Create a new sphere
        std::shared_ptr<Sphere> sphere = std::make_shared<Sphere>(
            position,
            radius,
            velocity,
            acceleration,
            fixed
        );

        return sphere;
}

std::shared_ptr<Molecule> parseMolecule(json j) {
    // first check the offset
    glm::vec3 offset = glm::vec3(0.0f);
    if (j.find("offset") != j.end()) {
        offset = glm::vec3(j["offset"][0], j["offset"][1], j["offset"][2]);
    }

    // check if a path to the molecule file was provided
    if (j.find("path") != j.end()) {
        std::ifstream file(j["path"]);
        if (!file.is_open()) {
            std::cerr << "Could not open file: " << j["path"] << std::endl;
            return nullptr;
        }
        j = json::parse(file);
    }

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
        std::shared_ptr<Sphere> sphere = parseSphere(jSphere);
        sphere->position += offset;
        sphere->previous_position = sphere->position;
        molecule->addSphere(sphere);
        spheres.push_back(sphere);
    }

    // Iterate over the links in the molecule
    for (const auto& jLink : j["links"]) {
        molecule->addLink(
            spheres[jLink[0]],
            spheres[jLink[1]]
        );
    }

    return molecule;
}

std::shared_ptr<Container> parseContainer(json j) {
    // init the parameters
    std::string type;
    glm::vec3 position;
    glm::vec3 size; // either size or radius
    float radius;
    bool forcedInside = false;

    // check for optional parameters
    if (j.find("forcedInside") != j.end()) {
        forcedInside = j["forcedInside"];
    }

    // getting required parameters
    type = j["type"];
    position = glm::vec3(j["position"][0], j["position"][1], j["position"][2]);
    if (type == "cube") {
        size = glm::vec3(j["size"][0], j["size"][1], j["size"][2]);
    } else if (type == "sphere") {
        radius = j["radius"];
    } else {
        std::cerr << "Unknown container type: " << type << std::endl;
        return nullptr;
    }

    // Create a new container
    std::shared_ptr<Container> container;
    if (type == "cube") {
        container = std::make_shared<CubeContainer>(
            position,
            size,
            forcedInside
        );
    } else if (type == "sphere") {
        container = std::make_shared<SphereContainer>(
            position,
            glm::vec3(radius),
            forcedInside
        );
    }

    return container;
}