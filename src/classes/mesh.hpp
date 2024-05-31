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
    GLuint VAO, VBO, VBOPosition, VBOscale, VBOrot;
    std::vector<Vertex> vertices;

    Mesh(const std::string &filename, bool instanced = false, bool single = false, bool oriented = false);

    void setupMesh(bool instanced = false, bool single = false, bool oriented = false);

    void draw(GLuint& ShaderProgram, const Camera &camera, std::vector<glm::vec3>& positions, std::vector<glm::vec3>& scales);
    void drawOriented(GLuint& ShaderProgram, const Camera &camera, std::vector<glm::vec3>& positions, std::vector<glm::vec3>& scales, std::vector<glm::vec3>& rotations);

private:
    void loadFromFile(const std::string &filename);
    void createSubVBO(GLuint &VBO, GLuint attributeIndex, GLsizei size, GLint numPerVertex, GLsizei stride, const void* pointer, const void* offset, GLuint divisor);
};