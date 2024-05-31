#pragma once

#include <glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "../classes/particle.hpp"
#include <memory>
#include "../classes/camera.hpp"
#include "../classes/simulation.hpp"
#include "ray.hpp"

class DragParticles { // ! only for spheres for the moment

    private:
        GLFWwindow* window;
        glm::vec2 lastMousePos;
        bool isDragging = false;
        std::shared_ptr<Sphere> draggedParticle;
        float dragDistance = 0.0f;
        bool fixedDrag = true; // fixedDrag is used to get a precise position for the dragged particle instead of adding a kind of force to the particle

    public:
        DragParticles(GLFWwindow* window, bool fixedDrag = true);
        void setDraggedParticle(std::shared_ptr<Sphere> particle);
        void unsetDraggedParticle();
        void setDragDistance(float dragDistance);
        void setWindow(GLFWwindow* window);
        void setLastMousePos(glm::vec2 pos);
        void setIsDragging(bool isDragging);
        
        void handleDrag(const Camera &camera, Simulation &simulation);
};
