#pragma once

#include <vector>
#include "particle.hpp"
#include "plane.hpp"
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
    void drawPlanes(const Camera& camera, const std::vector<Plane>& planes);
    GLuint createShaderProgram(const std::string& vertexShaderFile, const std::string& fragmentShaderFile);
    GLuint createShaderProgram(const std::string& vertexShaderFile, const std::string& geometryShaderFile, const std::string& fragmentShaderFile);
};