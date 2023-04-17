#version 410

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 VertexTexCoord;

// Tangets VAO at 3
layout (location = 4) in vec3 VertexColor;

layout(location=0) out vec3 Position;
layout(location=1) out vec3 Normal;
layout(location=2) out vec3 Color;
layout(location=3) out vec3 TexCoord;

uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 MVP;
uniform mat4 ModelMatrix;


void main() {
    Normal = normalize( NormalMatrix * VertexNormal);
    Position = ( ModelViewMatrix * vec4(VertexPosition,1.0) ).xyz;
    Color = VertexColor;
    TexCoord = vec3(ModelMatrix * vec4(VertexTexCoord, 0, 1));

    gl_Position = MVP * vec4(VertexPosition,1.0);
}