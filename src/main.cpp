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

#define TARGET_FPS 60
#define NUM_SUBSTEPS 8

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

    // Simulation times
    int nbFrames = 0;
    float dt = 0.1f;
    float lastTime = (float)glfwGetTime();

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
    sim.createSphere(glm::vec3(0.0f, 1.5f, 0.0f), 0.15f, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    sim.createSphere(glm::vec3(0.6f, 2.5f, 0.0f), 0.15f, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));

    while (!glfwWindowShouldClose(window)) {

        // if user press G, add a new sphere
        if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
            // sim.createSphere(glm::vec3((rand() / (float)RAND_MAX) * 9.0f - 4.5f, 5.0f, (rand() / (float)RAND_MAX) * 9.0f - 4.5f), 0.5f, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));
            sim.createSphere(glm::vec3((rand()/ (float)RAND_MAX * 1.0f - 0.5f), 1.0f, (rand() / (float)RAND_MAX) * 1.0f - 0.5f), 0.15f, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));
        }

        // Handle camera motion
        handleCameraMotion(window, camera);

        // Clear the screen and depth buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Update simulation
        float substep_dt = dt / NUM_SUBSTEPS;
        for (int j = 0; j < NUM_SUBSTEPS; j++) {
            sim.checkCollisions();
            sim.addForce(glm::vec3(0.0f, -20.0f, 0.0f));  // gravity
            sim.step(substep_dt);
        }


        // Draw particles
        // convert the particles to spheres
        renderer.draw(camera, sim.particles);

        // Draw the floor
        renderer.drawPlanes(camera, sim.planes);

        // Swap buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();

        // Timing
        if (nbFrames % TARGET_FPS == 0) {
            float fps = 1.0f / dt;
            glfwSetWindowTitle(window, ("Particle Simulator | FPS: " + to_string(fps) + " | Number of Particles: " + to_string(sim.getNumParticles())).c_str());
        }
        // Wait until the next frame
        dt = (float)glfwGetTime() - lastTime;
        while (dt < 1.0f / TARGET_FPS) {
            dt = (float)glfwGetTime() - lastTime;
        }
        lastTime = (float)glfwGetTime();
        nbFrames++;
    }

    glfwTerminate();

    return 0;
}