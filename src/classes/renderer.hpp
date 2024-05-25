#pragma once

#include <vector>
#include "particle.hpp"
#include "plane.hpp"
#include "camera.hpp"
#include "mesh.hpp"
#include "container.hpp"
#include "molecule.hpp"
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
    GLuint moleculeLinksShaderProgram;

public:
    Renderer();
    void draw(const Camera& camera, const std::vector<std::shared_ptr<Sphere>>& spheres);
    void draw(const Camera& camera, const std::vector<std::shared_ptr<Sphere>>& spheres, Mesh& mesh);
    void drawMoleculeLinks(const Camera& camera, const std::vector<std::shared_ptr<Molecule>>& molecules, Mesh& mesh);
    void drawPlanes(const Camera& camera, const std::vector<std::shared_ptr<Plane>>& planes);
    void drawContainer(const Camera& camera, const std::vector<std::shared_ptr<CubeContainer>>& containers, Mesh& mesh);
    GLuint createShaderProgram(const std::string& vertexShaderFile, const std::string& fragmentShaderFile);
    GLuint createShaderProgram(const std::string& vertexShaderFile, const std::string& geometryShaderFile, const std::string& fragmentShaderFile);
};