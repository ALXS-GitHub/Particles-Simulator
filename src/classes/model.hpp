#pragma once

#include <glm/glm.hpp>
#include <glew.h>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include "mesh.hpp"


class Model {
public:
    std::vector<Vertex> vertices;

    Model(const std::string& filename) {
        loadFromFile(filename);
    }

    Mesh createMesh() {
        Mesh mesh(vertices);
        return mesh;
    }

private:
    void loadFromFile(const std::string& filename) {
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
};