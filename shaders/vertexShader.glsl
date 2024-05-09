#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in float aRadius;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out float radius;

void main()
{
    gl_Position = projectionMatrix * viewMatrix * vec4(aPos, 1.0);
    radius = aRadius;
}