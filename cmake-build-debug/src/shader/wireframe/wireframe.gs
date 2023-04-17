#version 410 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

layout(location=0) in vec3 PositionVectors[];
layout(location=1) in vec3 NormalVectors[];
layout(location=2) in vec3 ColorVectors[];
layout(location=3) in vec3 TexCoords[];

layout(location=0) out vec3 Position;
layout(location=1) out vec3 Normal;
layout(location=2) out vec3 Color;
layout(location=3) out vec3 TexCoord;

layout(location=4) out vec3 dist;

float line_width = 0.01;

float compute_h(vec2 pA, vec2 pB, vec2 pC){

    vec2 vBA = (pB - pA) / line_width;
    vec2 vCA = (pC - pA) / line_width;

    float area = abs(vBA.x * vCA.y - vBA.y * vCA.x);

    return area / length(vBA - vCA);;
}

void main() {

    for (int i = 0; i < gl_in.length(); i++) {

        // Get current point in 2D space
        vec4 point_3D = gl_in[i].gl_Position;
        vec2 point = point_3D.xy / point_3D.w;

        // Get next two points in 2D space
        vec4 point_2_3D = gl_in[(i + 1) % gl_in.length()].gl_Position;
        vec2 point_2 = point_2_3D.xy / point_2_3D.w;

        vec4 point_3_3D = gl_in[(i + 2) % gl_in.length()].gl_Position;
        vec2 point_3 = point_3_3D.xy / point_3_3D.w;


        float h = compute_h(point, point_2, point_3);

        dist = vec3(0.0, 0.0, 0.0);
        dist[i] = h;

        gl_Position = point_3D;

        Position = PositionVectors[i];
        Normal = NormalVectors[i];
        Color = ColorVectors[i];
        TexCoord = TexCoords[i];
        EmitVertex();
    }

    EndPrimitive();

}