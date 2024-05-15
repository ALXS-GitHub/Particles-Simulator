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

    Mesh(const std::string &filename, bool instanced = false, bool single = false);

    void setupMesh(bool instanced = false, bool single = false);

    void draw(GLuint& ShaderProgram, const Camera &camera, std::vector<glm::vec3>& positions, std::vector<glm::vec3>& scales);

private:
    void loadFromFile(const std::string &filename);
};