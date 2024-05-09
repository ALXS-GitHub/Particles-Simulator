#pragma once

#include <vector>
#include "particle.hpp"
#include "camera.hpp"
#include <glew.h>
#include <fstream>
#include <sstream>
#include <string>
#include <memory>

class Renderer {

private:
    GLuint shaderProgram;
    GLuint floorShaderProgram;
    GLuint floorVao;
    GLuint floorVbo;
    GLuint floorTexture;

public:
    Renderer();
    void draw(const Camera& camera, const std::vector<std::shared_ptr<Particle>>& particles);
    void createFloor(const std::string& textureFile);
    void drawFloor(const Camera& camera);
    GLuint createShaderProgram(const std::string& vertexShaderFile, const std::string& fragmentShaderFile);
    GLuint createShaderProgram(const std::string& vertexShaderFile, const std::string& geometryShaderFile, const std::string& fragmentShaderFile);
};