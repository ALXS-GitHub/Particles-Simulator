#version 330 core
out vec4 FragColor;

uniform sampler2D texture_diffuse1;

void main()
{    
    vec3 color = vec3(1.0f, 1.0f, 1.0f);
    FragColor = vec4(color, 1.0f);
}