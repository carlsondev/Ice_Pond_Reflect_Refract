#version 410

in vec3 Position;
in vec3 Normal;
in vec2 TexCoord;
in vec3 Color;

layout (location=0) out vec4 FragColor;

uniform samplerCube envCubeMap;

void main() {

    FragColor = texture(envCubeMap, Position);
}
