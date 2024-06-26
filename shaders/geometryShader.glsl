#version 330 core

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

in float radius[];

out vec2 TexCoords;

void main() {
    vec4 pos = gl_in[0].gl_Position;
    float r = radius[0];

    r = 2.421 * r;

    gl_Position = pos + vec4(-r, -r, 0.0, 0.0);
    TexCoords = vec2(0.0, 0.0);
    EmitVertex();

    gl_Position = pos + vec4(r, -r, 0.0, 0.0);
    TexCoords = vec2(1.0, 0.0);
    EmitVertex();

    gl_Position = pos + vec4(-r, r, 0.0, 0.0);
    TexCoords = vec2(0.0, 1.0);
    EmitVertex();

    gl_Position = pos + vec4(r, r, 0.0, 0.0);
    TexCoords = vec2(1.0, 1.0);
    EmitVertex();

    EndPrimitive();
}