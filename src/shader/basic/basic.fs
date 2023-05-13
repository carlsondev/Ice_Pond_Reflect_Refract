#version 410

in vec3 Position;
in vec3 Normal;
in vec3 Color;

layout (location=0) out vec4 FragColor;
layout (location=1) out vec3 FragNormal;

void main() {
    FragColor = vec4(Color, 1.0);
    FragNormal = -Normal; // Maybe make negative?
}
