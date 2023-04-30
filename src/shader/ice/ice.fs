#version 410

in vec3 Position;
in vec3 Normal;
in vec3 Color;

layout (location=0) out vec4 FragColor;

void main() {
    // Adjust alpha to 0 when enviornment map is used
    FragColor = vec4(Color, 0.1);
}
