#include <glew.h>
#include "camera_utils.hpp"
#include <GLFW/glfw3.h>
#include "../classes/camera.hpp"
#include <iostream>

void handleCameraMotion(GLFWwindow* window, Camera& camera) {

    // TODO make something to adjust the sensitivity of the controls

    // mouse control
    static double lastX = 0.0, lastY = 0.0;
    double x, y;
    glfwGetCursorPos(window, &x, &y);

    double dx = x - lastX;
    double dy = y - lastY;

    lastX = x;
    lastY = y;

    // if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
    //     camera.moveRight(static_cast<float>(dx) * -0.1f);
    //     camera.moveUp(static_cast<float>(dy) * -0.1f);
    // }

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        camera.rotateHorizontal(static_cast<float>(dx) * 0.1f);
        camera.rotateVertical(static_cast<float>(dy) * -0.1f);
    }

    // scroll control
    glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset) {
        Camera* camera = static_cast<Camera*>(glfwGetWindowUserPointer(window));
        if (camera) {
            camera->moveInDirection(static_cast<float>(yoffset) * 0.1f);
        } else {
            std::cerr << "Error: Camera pointer is null in scroll callback" << std::endl;
        }
    });

    // key controls
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            camera.moveForward(0.1f);
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            camera.moveForward(-0.1f);
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            camera.moveRight(0.1f);
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            camera.moveRight(-0.1f);
        }
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
            camera.moveY(0.1f);
        }
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
            camera.moveY(-0.1f);
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
            camera.rotateHorizontal(0.1f);
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            camera.rotateHorizontal(-0.1f);
        }
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
            camera.rotateVertical(0.1f);
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
            camera.rotateVertical(-0.1f);
        }
        // if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        //     camera.resetYaw();
        // }
}