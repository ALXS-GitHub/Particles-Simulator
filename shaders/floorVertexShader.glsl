#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec2 TexCoords;

void main()
{
    gl_Position = projectionMatrix * viewMatrix * vec4(aPos, 1.0);
    TexCoords = aTexCoords;
}