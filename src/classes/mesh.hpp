#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glew.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "camera.hpp"

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
};

class Mesh {
public:

    GLuint VAO, VBO, VBOPosition, VBOscale;
    std::vector<Vertex> vertices;

    Mesh (const std::vector<Vertex>& vertices) {
        this->vertices = vertices;
        setupMesh(true);
    }

    void setupMesh(bool instanced = false) {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &VBOPosition);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

        if (instanced) {
            glGenBuffers(1, &VBOPosition);
            glBindBuffer(GL_ARRAY_BUFFER, VBOPosition);
            glBufferData(GL_ARRAY_BUFFER, 10000 * sizeof(glm::vec3) * 3, NULL, GL_STREAM_DRAW);
            glEnableVertexAttribArray(3);
            glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
            glVertexAttribDivisor(3, 1);

            glGenBuffers(1, &VBOscale);
            glBindBuffer(GL_ARRAY_BUFFER, VBOscale);
            glBufferData(GL_ARRAY_BUFFER, 10000 * sizeof(float), NULL, GL_STREAM_DRAW);
            glEnableVertexAttribArray(4);
            glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void*)0);
            glVertexAttribDivisor(4, 1);
        }

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindVertexArray(0);
    }

    void draw(GLuint ShaderProgram, const Camera& camera, std::vector<glm::vec3> positions, std::vector<float> scales) {
        glUseProgram(ShaderProgram);

        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f), 800.0f / 800.0f, 0.1f, 100.0f);

        glUniformMatrix4fv(glGetUniformLocation(ShaderProgram, "viewMatrix"), 1, GL_FALSE, glm::value_ptr(view));
        GLint projectionMatrixLocation = glGetUniformLocation(ShaderProgram, "projectionMatrix");
        glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBOPosition);
        glBufferSubData(GL_ARRAY_BUFFER, 0, positions.size() * sizeof(glm::vec3), &positions[0]);

        glBindBuffer(GL_ARRAY_BUFFER, VBOscale);
        glBufferSubData(GL_ARRAY_BUFFER, 0, scales.size() * sizeof(float), &scales[0]);

        glDrawArraysInstanced(GL_TRIANGLES, 0, vertices.size(), positions.size());
        glBindVertexArray(0);
    }

};