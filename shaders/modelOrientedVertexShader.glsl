#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 instancePos;
layout (location = 4) in vec3 instanceScale;
layout (location = 5) in vec3 instanceRotation; // New input for the rotation matrix

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec3 fragmentPos;
out vec3 fragNormal;

mat4 rotationMatrix(vec3 rotation)
{
    float rotX = radians(rotation.x);
    float rotY = radians(rotation.y);
    float rotZ = radians(rotation.z);

    mat4 rotationX = mat4
    (
        vec4(1.0, 0.0, 0.0, 0.0),
        vec4(0.0, cos(rotX), -sin(rotX), 0.0),
        vec4(0.0, sin(rotX), cos(rotX), 0.0),
        vec4(0.0, 0.0, 0.0, 1.0)
    );

    mat4 rotationY = mat4
    (
        vec4(cos(rotY), 0.0, sin(rotY), 0.0),
        vec4(0.0, 1.0, 0.0, 0.0),
        vec4(-sin(rotY), 0.0, cos(rotY), 0.0),
        vec4(0.0, 0.0, 0.0, 1.0)
    );

    mat4 rotationZ = mat4
    (
        vec4(cos(rotZ), -sin(rotZ), 0.0, 0.0),
        vec4(sin(rotZ), cos(rotZ), 0.0, 0.0),
        vec4(0.0, 0.0, 1.0, 0.0),
        vec4(0.0, 0.0, 0.0, 1.0)
    );

    return rotationZ * rotationY * rotationX;
}

mat4 translationMatrix(vec3 translation)
{
    return mat4
    (
        vec4(1.0, 0.0, 0.0, 0.0),
        vec4(0.0, 1.0, 0.0, 0.0),
        vec4(0.0, 0.0, 1.0, 0.0),
        vec4(translation, 1.0)
    );
}

mat4 scaleMatrix(vec3 scale) {
    return mat4(
        vec4(scale.x, 0.0, 0.0, 0.0),
        vec4(0.0, scale.y, 0.0, 0.0),
        vec4(0.0, 0.0, scale.z, 0.0),
        vec4(0.0, 0.0, 0.0, 1.0)
    );

}

mat4 modelMatrix(vec3 translation, vec3 scale, vec3 rotation)
{
    return translationMatrix(translation) * rotationMatrix(rotation) * scaleMatrix(scale);
}

void main()
{
    mat4 model = modelMatrix(instancePos, instanceScale, instanceRotation);
    fragmentPos = vec3(model * vec4(aPos, 1.0));
    mat3 normalMatrix = mat3(rotationMatrix(instanceRotation));
    fragNormal = normalize(normalMatrix * aNormal);
    gl_Position = projectionMatrix * viewMatrix * model * vec4(aPos, 1.0);
}