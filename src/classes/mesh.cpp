#include "mesh.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glew.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "camera.hpp"

Mesh::Mesh (const std::string& filename, bool instanced) {
    loadFromFile(filename);
    setupMesh(instanced);
}

void Mesh::setupMesh(bool instanced) {
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

void Mesh::draw(GLuint ShaderProgram, const Camera& camera, std::vector<glm::vec3> positions, std::vector<float> scales) {
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

void Mesh::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    std::string line;

    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> texCoords;
    std::vector<glm::vec3> normals;

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;

        if (prefix == "v") {
            glm::vec3 position;
            iss >> position.x >> position.y >> position.z;
            positions.push_back(position);
        } else if (prefix == "vt") {
            glm::vec2 texCoord;
            iss >> texCoord.x >> texCoord.y;
            texCoords.push_back(texCoord);
        } else if (prefix == "vn") {
            glm::vec3 normal;
            iss >> normal.x >> normal.y >> normal.z;
            normals.push_back(normal);
        } else if (prefix == "f") {
            std::string vertexData[3];
            iss >> vertexData[0] >> vertexData[1] >> vertexData[2];

            for (const auto& data : vertexData) {
                std::istringstream viss(data);
                std::vector<int> indices(3);

                for (int& index : indices) {
                    viss >> index;
                    if (viss.peek() == '/') {
                        viss.ignore();
                    }
                }

                Vertex vertex;
                vertex.position = positions[indices[0] - 1];
                vertex.texCoords = texCoords[indices[1] - 1];
                vertex.normal = normals[indices[2] - 1];
                vertices.push_back(vertex);
            }
        }
    }
}