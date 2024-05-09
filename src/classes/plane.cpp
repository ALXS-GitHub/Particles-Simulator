#include "plane.hpp"
#include <glm/glm.hpp>
#include <glew.h>
#include <iostream>
#include <vector>
#include "../utils/texture_utils.hpp"

Plane::Plane(glm::vec3 position, glm::vec3 normal, glm::vec2 size)
    : position(position), normal(normal), size(size) {

    normal = glm::normalize(normal);
    
    glm::vec3 u;
    std::vector<float> vertices;
    // Create two perpendicular vectors on the plane
    if (glm::abs(glm::dot(normal, glm::vec3(1.0f, 0.0f, 0.0f))) < 0.0001f) {
        u = glm::normalize(glm::cross(normal, glm::vec3(1.0f, 0.0f, 0.0f)));
    } else {
        u = glm::normalize(glm::cross(normal, glm::vec3(0.0f, 1.0f, 0.0f)));
    }
        // Scale the vectors by the size of the plane
    glm::vec3 v = glm::cross(normal, u);
    u *= size.x;
    v *= size.y;
    vertices = {
        // positions          // texture coords
        (position + u + v).x, (position + u + v).y, (position + u + v).z, size.x, size.y,
        (position - u - v).x, (position - u - v).y, (position - u - v).z, 0.0f, 0.0f,
        (position - u + v).x, (position - u + v).y, (position - u + v).z, 0.0f, size.y,
        (position + u - v).x, (position + u - v).y, (position + u - v).z, size.x, 0.0f,
        (position + u + v).x, (position + u + v).y, (position + u + v).z, size.x, size.y,
        (position - u - v).x, (position - u - v).y, (position - u - v).z, 0.0f, 0.0f
    };
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);

    // Load floor texture
    texture = loadTexture(textureFile.c_str());

    // Set the texture wrapping/filtering options
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Unbind the texture
    glBindTexture(GL_TEXTURE_2D, 0);
}