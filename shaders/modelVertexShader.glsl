#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 instancePos;
layout (location = 4) in float instanceScale;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

// out vec3 fragmentPos;

// TODO compute the scale later
mat4 translationMatrix(vec3 translation, float scale)
{
    return mat4
    (
        vec4(scale, 0.0, 0.0, 0.0),
        vec4(0.0, scale, 0.0, 0.0),
        vec4(0.0, 0.0, scale, 0.0),
        vec4(translation, 1.0)
    );
}

void main()
{
    mat4 model = translationMatrix(instancePos, instanceScale);
    // fragmentPos = vec3(model * vec4(aPos, 1.0));
    gl_Position = projectionMatrix * viewMatrix * model * vec4(aPos, 1.0);
}