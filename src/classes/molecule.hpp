#pragma once

#include<glm/glm.hpp>
#include "particle.hpp"
#include <vector>
#include <memory>

class Molecule {

    private:
        float distance = 0.5f; // distance between the spheres centers
        float strength = 0.01f; // strength of the spring

    public:

        float internalPressure = 0.001f; // internal pressure of the molecule
        std::vector<std::shared_ptr<Sphere>> spheres;
        std::vector<std::pair<std::shared_ptr<Sphere>, std::shared_ptr<Sphere>>> links; // change this later to have multiple distances and strengths
        bool linksEnabled = false; // define if we should use the links to maintain the distance or not
        Molecule(float distance = 0.5f, bool linksEnabled = false, float strength = 0.01f, float internalPressure = 0.001f, bool useInternalPressure = false);
        void addSphere(std::shared_ptr<Sphere> sphere);
        void addLink(std::shared_ptr<Sphere> sphere1, std::shared_ptr<Sphere> sphere2);
        void maintainDistanceAll();
        void maintainDistanceLinks();
        void maintainDistance(std::shared_ptr<Sphere> sphere1, std::shared_ptr<Sphere> sphere2);
        bool useInternalPressure = false; // define if we should use the internal pressure to maintain the distance or not
        void addInternalPressure();
        
};