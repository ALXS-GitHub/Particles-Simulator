#version 330 core

in vec2 TexCoords;

out vec4 FragColor;

void main()
{
    float r = length(TexCoords * 2.0 - 1.0);
    if (r > 1.0)
        discard;

    FragColor = vec4(vec3(1.0 - r), 1.0);
}