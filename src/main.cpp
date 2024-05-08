// main.cpp
#include "classes/simulation.hpp"
#include "classes/renderer.hpp"
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

    Simulation sim;
    Renderer renderer;
    vector<Sphere> spheres;

    // Add some particles
    shared_ptr<Sphere> p = make_shared<Sphere>();
    p->position = glm::vec3(0.0f, 0.0f, 0.0f);  // position at the center
    p->previous_position = p->position;
    p->velocity = glm::vec3(0.0f, 0.0f, 0.0f);  // no velocity
    p->force = glm::vec3(0.0f, 0.0f, 0.0f);  // no force
    p->mass = 1.0f;
    p->radius = 0.2f;
    sim.particles.push_back(p);

    shared_ptr<Sphere> p2 = make_shared<Sphere>();
    p2->position = glm::vec3(0.2f, 0.2f, 0.0f);  // position at the center
    p2->previous_position = p2->position;
    p2->velocity = glm::vec3(0.0f, 0.0f, 0.0f);  // no velocity
    p2->force = glm::vec3(-0.0001f, 0.0f, -0.001f);  // no force
    p2->mass = 1.0f;
    p2->radius = 0.3f;
    sim.particles.push_back(p2);


    while (!glfwWindowShouldClose(window)) {
        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT);

        // Update simulation
        sim.step(0.01f);

        // Draw particles
        // convert the particles to spheres
        renderer.draw(sim.particles);

        // Swap buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}