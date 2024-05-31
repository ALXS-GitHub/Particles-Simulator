#include "drag_particles.hpp"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "../classes/particle.hpp"
#include <memory>
#include "../config.hpp"
#include "../classes/camera.hpp"
#include "../classes/simulation.hpp"
#include "ray.hpp"

DragParticles::DragParticles(GLFWwindow* window, bool fixedDrag) {
    this->window = window;
    this->fixedDrag = fixedDrag;
}

void DragParticles::setDraggedParticle(std::shared_ptr<Sphere> particle) {
    draggedParticle = particle;
    particle->setUpdatingEnabled(false);
}

void DragParticles::unsetDraggedParticle() {
    if (draggedParticle) {
        draggedParticle->setUpdatingEnabled(true);
    }
    draggedParticle = nullptr;
}

void DragParticles::setDragDistance(float dragDistance) {
    this->dragDistance = dragDistance;
}

void DragParticles::setWindow(GLFWwindow* window) {
    this->window = window;
}

void DragParticles::setLastMousePos(glm::vec2 pos) {
    lastMousePos = pos;
}

void DragParticles::setIsDragging(bool isDragging) {
    this->isDragging = isDragging;
}

void DragParticles::handleDrag(const Camera &camera, Simulation &simulation) {
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        double x, y;
        glfwGetCursorPos(window, &x, &y);
        glm::vec2 mousePos = glm::vec2(x, y);

        // Get the camera position and view matrix  
        glm::vec3 cameraPosition = camera.position;
        glm::mat4 viewMatrix = camera.getViewMatrix();
        glm::mat4 projectionMatrix = camera.getProjectionMatrix(45.0f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);

        if (!isDragging) {
            isDragging = true;
            lastMousePos = mousePos;

            // handle getting the particle to drag


            // Convert the 2D mouse position to normalized device coordinates
            glm::vec4 ray_clip = glm::vec4((2.0f * mousePos.x) / WINDOW_WIDTH - 1.0f, 1.0f - (2.0f * mousePos.y) / WINDOW_HEIGHT, -1.0f, 1.0f);
            glm::vec4 ray_eye = glm::inverse(projectionMatrix) * ray_clip;
            ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0f, 0.0f);
            glm::vec3 ray_world = glm::vec3(glm::inverse(viewMatrix) * ray_eye);

            // Create a ray from the camera position in the direction of the mouse click
            Ray ray(cameraPosition, glm::normalize(ray_world));

            // Check for intersections between the ray and the particles
            std::shared_ptr<Sphere> selectedParticle = nullptr;
            float minDistance = std::numeric_limits<float>::max();
            for (auto& particle : simulation.spheres) {
                float distance = ray.intersect(particle);
                if (distance < minDistance) {
                    selectedParticle = particle;
                    minDistance = distance;
                }
            }

            // If there's an intersection, set the intersected particle as the dragged particle
            if (selectedParticle) {
                setDraggedParticle(selectedParticle);
                setDragDistance(minDistance);
            }

        }

        if (draggedParticle) {
            if (!fixedDrag) {
                // Convert the 2D mouse position difference to a 3D movement
                glm::vec2 mousePosDiff = mousePos - lastMousePos;
                glm::vec3 movement = glm::vec3(mousePosDiff.x, -mousePosDiff.y, 0.0f); // Invert y-axis because screen coordinates are inverted

                // Calculate the distance from the camera to the particle
                float distance = glm::length(cameraPosition - draggedParticle->position);

                // Calculate a scale factor based on the distance
                float scaleFactor = distance / 10000.0f; // 10000.0f is an arbitrary value that works well for this purpose

                // Scale the movement to match your scene dimensions and the distance from the camera
                movement *= scaleFactor;

                // Apply the movement to the dragged particle
                draggedParticle->move(movement);

                // Update the last mouse position
                lastMousePos = mousePos;
            } else {
                // Convert the 2D mouse position to normalized device coordinates
                glm::vec4 ray_clip = glm::vec4((2.0f * mousePos.x) / WINDOW_WIDTH - 1.0f, 1.0f - (2.0f * mousePos.y) / WINDOW_HEIGHT, 1.0f, 1.0f);
                glm::vec4 ray_eye = glm::inverse(projectionMatrix) * ray_clip;
                ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0f, 0.0f);
                glm::vec3 ray_world = glm::normalize(glm::vec3(glm::inverse(viewMatrix) * ray_eye));

                // Calculate the new position of the particle
                glm::vec3 newPosition = camera.position + ray_world * dragDistance;

                // Move the particle to the new position
                draggedParticle->position = newPosition;

                // Update the last mouse position
                lastMousePos = mousePos;
            }
        }


    } else { // on release
        isDragging = false;
        unsetDraggedParticle();
    }
}

