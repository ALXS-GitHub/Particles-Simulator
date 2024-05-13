#include "renderer.hpp"
#include "particle.hpp"
#include "../utils/texture_utils.hpp"
#include "plane.hpp"
#include "camera.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <glew.h>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <vector>
#include <memory>
#include "mesh.hpp"

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
}

void Renderer::draw(const Camera& camera, const std::vector<std::shared_ptr<Particle>>& particles) { // note : shared_ptr (*) meaning we take the pointer to the particle (and this allow polymorphism if we don't use the pointer we cannot use children classes) and the & meaning we take the reference to the particle
    std::vector<float> data;
    for (const auto& particle : particles) {
        std::shared_ptr<Sphere> sphere = std::dynamic_pointer_cast<Sphere>(particle); // dynamic_pointer_cast is used to convert the pointer to the base class to the pointer to the derived class
        if (sphere) {
            // If the particle is a sphere, add its position and radius to the data
            data.push_back(sphere->position.x);
            data.push_back(sphere->position.y);
            data.push_back(sphere->position.z);
            data.push_back(sphere->radius); // TODO fix this later to render the sphere properly
        } else {
            // If the particle is not a sphere, add its position to the data and use a default radius
            data.push_back(particle->position.x);
            data.push_back(particle->position.y);
            data.push_back(particle->position.z);
            data.push_back(0.0f);  // default radius for non-spheres
        }
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

    glDrawArrays(GL_POINTS, 0, particles.size());

    // Clean up
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &vbo);
}

void Renderer::draw(const Camera& camera, const std::vector<std::shared_ptr<Sphere>>& sphere, Mesh& mesh) {
    std::vector<glm::vec3> positions;
    std::vector<float> scales;

    for (const auto& s : sphere) {
        positions.push_back(s->position);
        scales.push_back(s->radius);
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
