#version 410

in vec3 Position;
in vec3 Normal;
in vec2 TexCoord;
in vec3 Color;

in vec3 ReflectDir;

layout (location=0) out vec4 FragColor;

uniform samplerCube envCubeMap;

void main() {
    FragColor = texture(envCubeMap, ReflectDir);
    //FragColor = texture(envCubeMap, vec3(Adj_Position.x, Adj_Position.y, Adj_Position.z));
    //FragColor = vec4(Normal, 1.0);
    //FragColor = texture(envCubeMap, vec3(-Normal.x, Normal.y, -Normal.z));
}
