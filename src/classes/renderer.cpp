#include "renderer.hpp"
#include "particle.hpp"
#include "../utils/texture_utils.hpp"
#include "plane.hpp"
#include "camera.hpp"
#include "molecule.hpp"
#include "container.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <glew.h>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <vector>
#include <memory>
#include "mesh.hpp"
#include <omp.h>

Renderer::Renderer() {
    // Create a shader program
    shaderProgram = createShaderProgram("shaders/vertexShader.glsl", "shaders/geometryShader.glsl", "shaders/fragmentShader.glsl");
    if (shaderProgram == 0) {
        std::cerr << "Failed to create shader program" << std::endl;
        exit(EXIT_FAILURE);
    }
    floorShaderProgram = createShaderProgram("shaders/floorVertexShader.glsl", "shaders/floorFragmentShader.glsl");
    if (floorShaderProgram == 0) {
        std::cerr << "Failed to create floor shader program" << std::endl;
        exit(EXIT_FAILURE);
    }
    modelShaderProgram = createShaderProgram("shaders/modelVertexShader.glsl", "shaders/modelFragmentShader.glsl");
    if (shaderProgram == 0) {
        std::cerr << "Failed to create shader program" << std::endl;
        exit(EXIT_FAILURE);
    }
    containerShaderProgram = createShaderProgram("shaders/containerVertexShader.glsl", "shaders/containerFragmentShader.glsl");
    if (containerShaderProgram == 0) {
        std::cerr << "Failed to create container shader program" << std::endl;
        exit(EXIT_FAILURE);
    }
    moleculeLinksShaderProgram = createShaderProgram("shaders/modelOrientedVertexShader.glsl", "shaders/modelFragmentShader.glsl");
}

void Renderer::draw(const Camera& camera, const std::vector<std::shared_ptr<Sphere>>& spheres) { // note : shared_ptr (*) meaning we take the pointer to the particle (and this allow polymorphism if we don't use the pointer we cannot use children classes) and the & meaning we take the reference to the particle
    std::vector<float> data;
    for (const auto& sphere : spheres) { 
        data.push_back(sphere->position.x);
        data.push_back(sphere->position.y);
        data.push_back(sphere->position.z);
        data.push_back(sphere->radius); // TODO fix this later to render the sphere properly
    }

    // Create a VBO and copy the data into it
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);

    // Use the shader program
    glUseProgram(shaderProgram);

    // Set the view matrix uniform
    glm::mat4 viewMatrix = camera.getViewMatrix();
    GLint viewMatrixLocation = glGetUniformLocation(shaderProgram, "viewMatrix");
    glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    // Set the projection matrix uniform
    glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f), 800.0f / 800.0f, 0.1f, 100.0f);
    GLint projectionMatrixLocation = glGetUniformLocation(shaderProgram, "projectionMatrix");
    glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    // Set up the vertex attribute pointers and draw the particles
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(3 * sizeof(float)));

    glDrawArrays(GL_POINTS, 0, spheres.size());

    // Clean up
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &vbo);
}

void Renderer::draw(const Camera& camera, const std::vector<std::shared_ptr<Sphere>>& spheres, Mesh& mesh) {
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> scales;

    positions.reserve(spheres.size());
    scales.reserve(spheres.size());
    
    for (const auto& s : spheres) {
        positions.emplace_back(s->position);
        scales.emplace_back(glm::vec3(s->radius));
    }

    // Use the shader program
    mesh.draw(modelShaderProgram, camera, positions, scales);
}

GLuint Renderer::createShaderProgram(const std::string& vertexShaderFile, const std::string& fragmentShaderFile) {
    // Load the vertex shader
    std::ifstream vShaderFile("../" + vertexShaderFile);
    if (!vShaderFile) {
        std::cerr << "Unable to open file " << vertexShaderFile << std::endl;
        return 0;
    }
    std::stringstream vShaderStream;
    vShaderStream << vShaderFile.rdbuf();
    std::string vShaderStr = vShaderStream.str();
    const char* vShaderSrc = vShaderStr.c_str();

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vShaderSrc, nullptr);
    glCompileShader(vertexShader);

    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        return 0;
    }

    // Load the fragment shader
    std::ifstream fShaderFile("../" + fragmentShaderFile);
    if (!fShaderFile) {
        std::cerr << "Unable to open file " << fragmentShaderFile << std::endl;
        return 0;
    }
    std::stringstream fShaderStream;
    fShaderStream << fShaderFile.rdbuf();
    std::string fShaderStr = fShaderStream.str();
    const char* fShaderSrc = fShaderStr.c_str();

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fShaderSrc, nullptr);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        return 0;
    }

    // Create the shader program
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        return 0;
    }

    // Clean up
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

GLuint Renderer::createShaderProgram(const std::string& vertexShaderFile, const std::string& geometryShaderFile, const std::string& fragmentShaderFile) {
    // Load the vertex shader
    std::ifstream vShaderFile("../" + vertexShaderFile);
    if (!vShaderFile) {
        std::cerr << "Unable to open file " << vertexShaderFile << std::endl;
        return 0;
    }
    std::stringstream vShaderStream;
    vShaderStream << vShaderFile.rdbuf();
    std::string vShaderStr = vShaderStream.str();
    const char* vShaderSrc = vShaderStr.c_str();

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vShaderSrc, nullptr);
    glCompileShader(vertexShader);

    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        return 0;
    }

    // Load the geometry shader
    std::ifstream gShaderFile("../" + geometryShaderFile);
    if (!gShaderFile) {
        std::cerr << "Unable to open file " << geometryShaderFile << std::endl;
        return 0;
    }
    std::stringstream gShaderStream;
    gShaderStream << gShaderFile.rdbuf();
    std::string gShaderStr = gShaderStream.str();
    const char* gShaderSrc = gShaderStr.c_str();

    GLuint geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
    glShaderSource(geometryShader, 1, &gShaderSrc, nullptr);
    glCompileShader(geometryShader);

    glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(geometryShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::GEOMETRY::COMPILATION_FAILED\n" << infoLog << std::endl;
        return 0;
    }

    // Load the fragment shader
    std::ifstream fShaderFile("../" + fragmentShaderFile);
    if (!fShaderFile) {
        std::cerr << "Unable to open file " << fragmentShaderFile << std::endl;
        return 0;
    }
    std::stringstream fShaderStream;
    fShaderStream << fShaderFile.rdbuf();
    std::string fShaderStr = fShaderStream.str();
    const char* fShaderSrc = fShaderStr.c_str();

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fShaderSrc, nullptr);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        return 0;
    }

    // Create the shader program
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, geometryShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        return 0;
    }

    // Clean up
    glDeleteShader(vertexShader);
    glDeleteShader(geometryShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

void Renderer::drawMoleculeLinks(const Camera& camera, const std::vector<std::shared_ptr<Molecule>>& molecules, Mesh& mesh) {
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> scales;
    std::vector<glm::vec3> rotations; // New vector for the rotation matrices

    for (const auto& molecule : molecules) {
        for (int i = 0; i < molecule->links.size(); i++) {
            std::shared_ptr<Sphere> s1 = molecule->links[i].first;
            std::shared_ptr<Sphere> s2 = molecule->links[i].second;
            glm::vec3 center = (s1->position + s2->position) / 2.0f;
            float distance = glm::length(s1->position - s2->position);
            positions.push_back(center);
            scales.push_back(glm::vec3(s1->radius / 2, distance / 2, s1->radius / 2));
            // scales.push_back(glm::vec3(0.25f));

            // Calculate the direction vector of the link
            glm::vec3 axis = glm::normalize(s2->position - s1->position);
            glm::vec3 modelUp = glm::vec3(0.0f, 1.0f, 0.0f);
            glm::vec3 modelX = glm::vec3(1.0f, 0.0f, 0.0f);
            glm::vec3 modelZ = glm::vec3(0.0f, 0.0f, 1.0f);

            // get the dot product of the axis and modelUp vectors
            float dotProduct = glm::dot(axis, modelUp);

            // clamp the value to the range [-1.0, 1.0] to avoid precision issues
            dotProduct = glm::clamp(dotProduct, -1.0f, 1.0f);

            // get the angle between the up axis and the axis
            float angleX = glm::degrees(acos(dotProduct));

            // get the cross product of the axis and modelUp vectors
            glm::vec3 crossProductX = glm::cross(axis, modelUp);

            // adjust the sign of the angle based on the direction of the cross product
            if (glm::dot(modelUp, crossProductX) < 0) {
                angleX = -angleX;
            }
            
            // get the projection of the axis on the xz plane
            glm::vec3 axisXZ = glm::normalize(axis - glm::dot(axis, modelUp) * modelUp);

            // get the cross product of the axisXZ and modelZ vectors
            glm::vec3 crossProduct = glm::cross(axisXZ, modelZ);

            // get the angle between the z axis and the projection of the axis on the xz plane
            float angleY = glm::degrees(atan2(glm::length(crossProduct), glm::dot(axisXZ, modelZ)));

            // check nan
            if (isnan(angleY)) {
                angleY = 0.0f;
            }

            if (glm::dot(modelUp, crossProduct) < 0) {
                angleY = -angleY;
            }

            angleY = angleY + 180.0f;

            glm::vec3 rotation = glm::vec3(angleX, angleY, 0.0f);
            rotations.push_back(rotation); // Add the rotation angles to the vector

        }
    }

    mesh.drawOriented(moleculeLinksShaderProgram, camera, positions, scales, rotations); // Pass the rotation matrices to the draw function
}

void Renderer::drawPlanes(const Camera& camera, const std::vector<std::shared_ptr<Plane>>& planes) {
    // Use the shader program
    glUseProgram(floorShaderProgram);

    // Set the view and projection matrix uniforms
    glm::mat4 viewMatrix = camera.getViewMatrix();
    glUniformMatrix4fv(glGetUniformLocation(floorShaderProgram, "viewMatrix"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
    glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f), 800.0f / 800.0f, 0.1f, 100.0f);
    glUniformMatrix4fv(glGetUniformLocation(floorShaderProgram, "projectionMatrix"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    for (const auto& plane : planes) {
        // Bind the floor texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, plane->getTexture());

        // Set the texture uniform
        glUniform1i(glGetUniformLocation(floorShaderProgram, "texture1"), 0);

        // Bind the floor VAO and draw the floor
        glBindVertexArray(plane->getVao());
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Unbind the VAO and the texture
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

void Renderer::drawContainer(const Camera& camera, const std::vector<std::shared_ptr<CubeContainer>>& containers, Mesh& mesh) {
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> scales;

    for (const auto& container : containers) {
        positions.push_back(container->position);
        scales.push_back(container->size);
    }

    // Use the shader program
    mesh.draw(containerShaderProgram, camera, positions, scales);
}
