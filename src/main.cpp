// main.cpp
#include "classes/simulation.hpp"
#include "classes/renderer.hpp"
#include "classes/camera.hpp"
#include "classes/plane.hpp"
#include "classes/particle.hpp"
#include "classes/molecule.hpp"
#include "utils/texture_utils.hpp"
#include "utils/camera_utils.hpp"
#include "dependencies/glew/glew.h"
#include "classes/mesh.hpp"
#include <GLFW/glfw3.h>
#include <iostream>
#include <memory>

#define TARGET_FPS 60
#define NUM_SUBSTEPS 8
#define ADD_PARTICLE_NUM 10

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

    // Enable face culling (to hide the back faces of the cubes)
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    // transparency blending function
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Change the background color to sky blue
    glClearColor(0.53f, 0.81f, 0.92f, 1.0f);

    // disable vsync
    glfwSwapInterval(0);

    // Simulation times
    int nbFrames = 0;
    float dt = 0.1f;
    float lastTime = (float)glfwGetTime();

    Simulation sim;
    Renderer renderer;

    // load a model
    Mesh mesh = Mesh("../models/sphere_ico_low.obj", true, false);
    // mesh.addPosition(glm::vec3(0.0f, 1.0f, 0.0f));
    
    // load the mesh for the container
    Mesh containerMesh = Mesh("../models/cube.obj", true, true);

    // load the particles link mesh
    Mesh linkMesh = Mesh("../models/cylinder.obj", true, false, true);

    // setting up the planes
    // shared_ptr<Plane> floor = make_shared<Plane>(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-0.0f, 1.0f, 0.0f), glm::vec2(10.0f, 10.0f));
    // sim.planes.push_back(floor);
    // shared_ptr<Plane> leftWall = make_shared<Plane>(glm::vec3(-5.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(10.0f, 10.0f));
    // sim.planes.push_back(leftWall);
    // shared_ptr<Plane> rightWall = make_shared<Plane>(glm::vec3(5.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(10.0f, 10.0f));
    // sim.planes.push_back(rightWall);
    // shared_ptr<Plane> backWall = make_shared<Plane>(glm::vec3(0.0f, 0.0f, -5.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(10.0f, 10.0f));
    // sim.planes.push_back(backWall);
    // shared_ptr<Plane> frontWall = make_shared<Plane>(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(10.0f, 10.0f));
    // sim.planes.push_back(frontWall);

    // Create a camera
    glm::vec3 cameraPosition = glm::vec3(0.0f, 1.0f, 15.0f);  // position the camera in front of the particles
    glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, -1.0f);  // looking towards the negative z-axis
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); // y-axis is up
    Camera camera(cameraPosition, cameraDirection, cameraUp);

    // Add some spheres
    // sim.createSphere(glm::vec3(0.0f, 1.5f, 0.0f), 0.15f, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), true);
    // sim.createSphere(glm::vec3(0.6f, 2.5f, 0.0f), 0.55f, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    // sim.createSphere(glm::vec3(-0.6f, 4.5f, 0.0f), 1.5f, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));

    // Add a molecule
    // shared_ptr<Molecule> molecule = make_shared<Molecule>(1.0f, true, 0.01f);
    // shared_ptr<Sphere> s1 = sim.createSphere(glm::vec3(0.0f, 1.5f, 0.0f), 0.15f, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    // shared_ptr<Sphere> s2 = sim.createSphere(glm::vec3(0.0f, 1.5f, 1.0f), 0.15f, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    // shared_ptr<Sphere> s3 = sim.createSphere(glm::vec3(1.0f, 1.5f, 0.0f), 0.15f, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    // shared_ptr<Sphere> s4 = sim.createSphere(glm::vec3(0.0f, 2.5f, 0.0f), 0.15f, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    // // shared_ptr<Sphere> s5 = sim.createSphere(glm::vec3(0.5f, 2.0f, 0.1f), 0.15f, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));

    // molecule->addSphere(s1);
    // molecule->addSphere(s2);
    // molecule->addSphere(s3);
    // molecule->addSphere(s4);
    // // molecule->addSphere(s5);

    // molecule->addLink(s1, s2);
    // molecule->addLink(s1, s3);
    // molecule->addLink(s1, s4);
    // molecule->addLink(s2, s3);
    // molecule->addLink(s2, s4);
    // molecule->addLink(s3, s4);

    // sim.molecules.push_back(molecule);

    sim.loadMolecule("../data/icosphere.json");
    sim.loadMolecule("../data/icosphere.json", glm::vec3(2.0f, 0.0f, 3.0f));

    // setting up the container
    sim.createCubeContainer(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(10.0f, 10.0f, 10.0f), true);

    while (!glfwWindowShouldClose(window)) {

        // if user press G, add a new sphere
        if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
            for (int i = 0; i < ADD_PARTICLE_NUM; i++)
                // sim.createSphere(glm::vec3((rand()/ (float)RAND_MAX * 1.0f - 0.5f), 1.0f, (rand() / (float)RAND_MAX) * 1.0f - 0.5f), 0.15f, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));
                sim.createSphere(glm::vec3((rand() / (float)RAND_MAX) * 9.0f - 4.5f, 2.0f, (rand() / (float)RAND_MAX) * 9.0f - 4.5f), 0.15f, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));
        }

        // if use press T, attract all the particles to the center and counteract gravity
        if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
            for (auto& p : sim.particles) {
                p->addForce(glm::vec3(0.0f, 10.0f, 0.0f) * (float)NUM_SUBSTEPS); // not very accurate since the force is applied multiple times in the in the same frame (but it's good enough for this purpose)
                p->addForce(-glm::normalize(p->position) * 50.0f * (float)NUM_SUBSTEPS);
            }
        }

        // Handle camera motion
        handleCameraMotion(window, camera);

        // Clear the screen and depth buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Check if 'p' key is pressed (pause the simulation)
        if (glfwGetKey(window, GLFW_KEY_P) != GLFW_PRESS) {
            // Update simulation
            float substep_dt = dt / NUM_SUBSTEPS;
            for (int j = 0; j < NUM_SUBSTEPS; j++) {
                // sim.checkCollisions();
                sim.checkGridCollisions();
                sim.maintainMolecules();
                sim.addForce(glm::vec3(0.0f, -10.0f, 0.0f));  // gravity
                sim.step(substep_dt);
            }
        }


        // Draw particles
        // convert the particles to spheres
        renderer.draw(camera, sim.spheres, mesh);
        // renderer.draw(camera, sim.spheres); // using a more efficient shader that doesn't require a model

        renderer.drawMoleculeLinks(camera, sim.molecules, linkMesh);

        // Draw the floor
        renderer.drawPlanes(camera, sim.planes);
        // mesh.draw(renderer.modelShaderProgram, camera, {glm::vec3(3.0f, 1.0f, 0.0f)});

        glEnable(GL_BLEND); // enable transparency
        renderer.drawContainer(camera, sim.containers, containerMesh);
        glDisable(GL_BLEND); // disable transparency

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