#pragma once

#include <vector>
#include "particle.hpp"
#include <glew.h>
#include <fstream>
#include <sstream>
#include <string>

class Renderer {

private:
    GLuint shaderProgram;

public:
    Renderer();
    void draw(const std::vector<std::shared_ptr<Particle>>& particles);
    GLuint createShaderProgram(const std::string& vertexShaderFile, const std::string& geometryShaderFile, const std::string& fragmentShaderFile);
};