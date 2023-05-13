#version 410

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 VertexTexCoord;
layout (location = 4) in vec3 VertexColor;

out vec3 Position;
out vec3 Normal;
out vec2 TexCoord;
out vec3 Color;

out vec3 ReflectDir;

uniform vec3 CameraPosition;

uniform mat4 ModelViewMatrix;
uniform mat4 ModelMatrix;
uniform mat3 NormalMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 MVP;

void main() {
    Normal = normalize( NormalMatrix * VertexNormal);

    vec3 worldPos = vec3( ModelMatrix * vec4(VertexPosition,1.0) );
    vec3 worldNorm = vec3(ModelMatrix * vec4(VertexNormal, 0.0));
    vec3 worldView = normalize( CameraPosition - worldPos );

    ReflectDir = reflect(-worldView, worldNorm );

    Position = VertexPosition;
    TexCoord = VertexTexCoord;
    Color = VertexColor;
    gl_Position = MVP * vec4(VertexPosition,1.0);
}