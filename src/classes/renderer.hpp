#pragma once

#include <vector>
#include "particle.hpp"
#include "plane.hpp"
#include "camera.hpp"
#include "mesh.hpp"
#include "container.hpp"
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
    GLuint modelShaderProgram;
    GLuint containerShaderProgram;

public:
    Renderer();
    void draw(const Camera& camera, const std::vector<std::shared_ptr<Particle>>& particles);
    void draw(const Camera& camera, const std::vector<std::shared_ptr<Sphere>>& sphere, Mesh& mesh);
    void drawPlanes(const Camera& camera, const std::vector<std::shared_ptr<Plane>>& planes);
    void drawContainer(const Camera& camera, const std::vector<std::shared_ptr<CubeContainer>>& containers, Mesh& mesh);
    GLuint createShaderProgram(const std::string& vertexShaderFile, const std::string& fragmentShaderFile);
    GLuint createShaderProgram(const std::string& vertexShaderFile, const std::string& geometryShaderFile, const std::string& fragmentShaderFile);
};