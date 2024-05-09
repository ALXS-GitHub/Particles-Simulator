// main.cpp
#include "classes/simulation.hpp"
#include "classes/renderer.hpp"
#include "classes/camera.hpp"
#include "classes/plane.hpp"
#include "classes/particle.hpp"
#include "utils/texture_utils.hpp"
#include "utils/camera_utils.hpp"
#include "dependencies/glew/glew.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <memory>

using namespace std;

int main() {
    cout << "Hello, World!" << endl;
    cout.flush();
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(800, 800, "Particle Simulator", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;  // Needed for core profile
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

    // Change the background color to sky blue
    glClearColor(0.53f, 0.81f, 0.92f, 1.0f);


    Simulation sim;
    Renderer renderer;
    vector<Sphere> spheres;
    vector<Plane> planes;

    Plane floor(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(10.0f, 10.0f));
    planes.push_back(floor);
    Plane frontWall(glm::vec3(5.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(10.0f, 10.0f));
    planes.push_back(frontWall);
    Plane rightWall(glm::vec3(5.0f, 0.0f, 5.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(10.0f, 10.0f));
    planes.push_back(rightWall);

    // Create a camera
    glm::vec3 cameraPosition = glm::vec3(0.0f, 1.0f, 5.0f);  // position the camera in front of the particles
    glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, -1.0f);  // looking towards the negative z-axis
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); // y-axis is up
    Camera camera(cameraPosition, cameraDirection, cameraUp);

    // Add some particles
    shared_ptr<Sphere> p = make_shared<Sphere>();
    p->position = glm::vec3(0.0f, 1.0f, 0.0f);  // position at the center
    p->previous_position = p->position;
    p->velocity = glm::vec3(0.0f, 0.0f, 0.0f);  // no velocity
    p->acceleration = glm::vec3(0.0f, 0.0f, 0.0f);  // no acceleration
    p->radius = 0.2f;
    sim.particles.push_back(p);

    shared_ptr<Sphere> p2 = make_shared<Sphere>();
    p2->position = glm::vec3(0.2f, 0.2f, 0.0f);  // position 
    p2->previous_position = p2->position;
    p2->velocity = glm::vec3(0.0f, 0.0f, 0.0f);  // no velocity
    p2->acceleration = glm::vec3(0.0f, 0.0f, 0.0f);  // no acceleration
    p2->radius = 0.3f;
    sim.particles.push_back(p2);

    while (!glfwWindowShouldClose(window)) {
        // Handle camera motion
        handleCameraMotion(window, camera);

        // Clear the screen and depth buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Update simulation
        // p->addForce(glm::vec3(0.0f, -0.001f, 0.0f));  // gravity
        sim.step(0.01f);

        // Draw particles
        // convert the particles to spheres
        renderer.draw(camera, sim.particles);

        // Draw the floor
        renderer.drawPlanes(camera, planes);

        // Swap buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}