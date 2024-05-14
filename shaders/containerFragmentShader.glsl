#version 330 core
out vec4 FragColor;

uniform sampler2D texture_diffuse1;

in vec3 fragmentPos;
in vec3 fragNormal;

vec3 diffuseLighting(vec3 normal, vec3 lightColor, vec3 lightPos, vec3 viewDir)
{
    // diffuse lighting
    vec3 lightDir = normalize(lightPos - fragmentPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    return diffuse;
}

void main()
{    
    // ambient lighting
    float ambientStrength = 0.5f;

    // diffuse lighting
    vec3 lightColor = vec3(1.0f, 0.5f, 1.0f);
    vec3 lightPos = vec3(30.0f, 55.0f, 105.0f);
    vec3 viewDir = normalize(vec3(0.0f, 0.0f, 0.0f) - fragmentPos);
    vec3 normal = normalize(fragNormal);
    vec3 diffuse = diffuseLighting(normal, lightColor, lightPos, viewDir);

    // vec3 ambientColor = lightColor * ambientStrength;

    vec3 color = vec3(0.8f, 0.8f, 0.8f);

    FragColor = vec4(color * (ambientStrength + diffuse), 0.3f);
}