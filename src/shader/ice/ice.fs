#version 410

in vec3 Position;
in vec3 Normal;
in vec2 TexCoord;
in vec3 Color;

layout (location=0) out vec4 FragColor;

uniform sampler2D posX;
uniform sampler2D negX;
uniform sampler2D posY;
uniform sampler2D negY;
uniform sampler2D posZ;
uniform sampler2D negZ;

void main() {
    // Adjust alpha to 0 when enviornment map is used

    float side_len = 1;

    vec2 adj_pos_xz = (Position.xz + vec2(2.5))/5.0;
    vec2 adj_pos_xz_uv = vec2(adj_pos_xz.x,  adj_pos_xz.y);

    vec2 adj_pos_yz = (Position.zy + vec2(2.5))/5.0;
    vec2 adj_pos_yz_uv = vec2(1- adj_pos_yz.x, adj_pos_yz.y);

    vec2 adj_pos_xy = (Position.xy + vec2(2.5))/5.0;
    vec2 adj_pos_xy_uv = vec2(1 - adj_pos_xy.x, adj_pos_xy.y);

    if (Normal.y == 1) {
        FragColor = texture(posY, adj_pos_xz_uv);
        return;
    }
    if (Normal.y == -1) {
        FragColor = texture(negY, adj_pos_xz_uv);
        return;
    }
    if (Normal.x == 1) {
        FragColor = texture(posX, adj_pos_yz);
        return;
    }
    if (Normal.x == -1) {
        FragColor = texture(negX, adj_pos_yz_uv);
        return;
    }
    if (Normal.z == 1) {
        FragColor = texture(posZ, adj_pos_xy_uv);
        return;
    }
    if (Normal.z == -1) {
        FragColor = texture(negZ, adj_pos_xy);
        return;
    }


    FragColor = vec4(Color, 0.1);
}
