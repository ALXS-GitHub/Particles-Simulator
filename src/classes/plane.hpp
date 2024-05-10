#pragma once

#include <glm/glm.hpp>
#include <glew.h>
#include <iostream>
#include <string>

class Plane {
    private:
        glm::vec3 position; // Center of the plane
        glm::vec3 normal; // Normal vector to the plane
        glm::vec2 size; // Size of the plane
        GLuint vao;
        GLuint vbo;
        GLuint texture;
        std::string textureFile = "../assets/floor.png";

    public:
        Plane(glm::vec3 position, glm::vec3 normal, glm::vec2 size);

        glm::vec3 getPosition() const {
            return position;
        }

        glm::vec3 getNormal() const {
            return normal;
        }

        glm::vec2 getSize() const {
            return size;
        }

        GLuint getVao() const {
            return vao;
        }

        GLuint getVbo() const {
            return vbo;
        }

        GLuint getTexture() const {
            return texture;
        }
};