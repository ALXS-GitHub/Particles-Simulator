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

    double lastTime = glfwGetTime();
    int nbFrames = 0;

    Simulation sim;
    Renderer renderer;

    shared_ptr<Plane> floor = make_shared<Plane>(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-0.0f, 1.0f, 0.0f), glm::vec2(10.0f, 10.0f));
    sim.planes.push_back(floor);
    shared_ptr<Plane> leftWall = make_shared<Plane>(glm::vec3(-5.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(10.0f, 10.0f));
    sim.planes.push_back(leftWall);
    shared_ptr<Plane> rightWall = make_shared<Plane>(glm::vec3(5.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(10.0f, 10.0f));
    sim.planes.push_back(rightWall);
    shared_ptr<Plane> backWall = make_shared<Plane>(glm::vec3(0.0f, 0.0f, -5.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(10.0f, 10.0f));
    sim.planes.push_back(backWall);
    shared_ptr<Plane> frontWall = make_shared<Plane>(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(10.0f, 10.0f));
    sim.planes.push_back(frontWall);

    // Create a camera
    glm::vec3 cameraPosition = glm::vec3(0.0f, 1.0f, 5.0f);  // position the camera in front of the particles
    glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, -1.0f);  // looking towards the negative z-axis
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); // y-axis is up
    Camera camera(cameraPosition, cameraDirection, cameraUp);

    // Add some spheres
    sim.createSphere(glm::vec3(0.0f, 1.5f, 0.0f), 0.5f, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    sim.createSphere(glm::vec3(0.6f, 2.5f, 0.0f), 0.5f, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));

    while (!glfwWindowShouldClose(window)) {

        // Measure speed
        double currentTime = glfwGetTime();
        nbFrames++;
        if ( currentTime - lastTime >= 1.0 ){ // If last prinf() was more than 1 sec ago
            // printf and reset timer
            char title[256];
            title[255] = '\0';
            snprintf(title, 255, "Particle Simulator [FPS: %d], [Particles: %lu]", nbFrames, sim.particles.size());
            glfwSetWindowTitle(window, title);
            nbFrames = 0;
            lastTime += 1.0;
        }

        // if user press G, add a new sphere
        if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
            shared_ptr<Sphere> p = make_shared<Sphere>();
            p->position = glm::vec3((rand() / (float)RAND_MAX) * 9.0f - 4.5f, 5.0f, (rand() / (float)RAND_MAX) * 9.0f - 4.5f);  // random position
            p->previous_position = p->position;
            p->velocity = glm::vec3(0.0f, 0.0f, 0.0f);  // no velocity
            p->acceleration = glm::vec3(0.0f, 0.0f, 0.0f);  // no acceleration
            p->radius = 0.5f;
            sim.particles.push_back(p);
        }

        // Handle camera motion
        handleCameraMotion(window, camera);

        // Clear the screen and depth buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Update simulation
        sim.checkCollisions();
        sim.addForce(glm::vec3(0.0f, -10.0f, 0.0f));  // gravity
        sim.step(0.05f);

        // Draw particles
        // convert the particles to spheres
        renderer.draw(camera, sim.particles);

        // Draw the floor
        renderer.drawPlanes(camera, sim.planes);

        // Swap buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}