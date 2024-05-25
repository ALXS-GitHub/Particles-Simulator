#pragma once

#include<glm/glm.hpp>
#include "particle.hpp"
#include <vector>
#include <memory>

class Molecule {

    private:
        std::vector<std::shared_ptr<Sphere>> spheres;
        float distance = 0.5f; // distance between the spheres centers
        float strength = 0.01f; // strength of the spring

    public:

        std::vector<std::pair<std::shared_ptr<Sphere>, std::shared_ptr<Sphere>>> links; // change this later to have multiple distances and strengths
        bool linksEnabled = false; // define if we should use the links to maintain the distance or not
        Molecule(float distance = 0.5f, bool linksEnabled = false, float strength = 0.01f);
        void addSphere(std::shared_ptr<Sphere> sphere);
        void addLink(std::shared_ptr<Sphere> sphere1, std::shared_ptr<Sphere> sphere2);
        void maintainDistanceAll();
        void maintainDistanceLinks();
        void maintainDistance(std::shared_ptr<Sphere> sphere1, std::shared_ptr<Sphere> sphere2);
        
};