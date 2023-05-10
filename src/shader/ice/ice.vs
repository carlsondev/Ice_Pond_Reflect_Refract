#version 410

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 VertexTexCoord;
layout (location = 4) in vec3 VertexColor;

out vec3 Position;
out vec3 Normal;
out vec2 TexCoord;
out vec3 Color;

uniform mat4 ModelViewMatrix;
uniform mat4 ModelMatrix;
uniform mat3 NormalMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 MVP;

void main() {
    Normal = normalize( NormalMatrix * VertexNormal);
    Position =VertexPosition;
    TexCoord = VertexTexCoord;
    Color = VertexColor;
    gl_Position = MVP * vec4(VertexPosition,1.0);
}