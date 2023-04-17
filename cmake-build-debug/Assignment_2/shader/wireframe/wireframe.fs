#version 410 core


layout(location=0) in vec3 Position;
layout(location=1) in vec3 Normal;
layout(location=2) in vec3 Color;
layout(location=3) in vec3 TexCoord;
layout(location=4) in vec3 dist;


out vec4 frag_color;

vec3 gradients[16];
int table[16];

void init() {
    int i;
    gradients[0] = vec3(0, -1, -1);
    gradients[1] = vec3(1, 0, -1);
    gradients[2] = vec3(0, -1, 1);
    gradients[3] = vec3(0, 1, -1);
    gradients[4] = vec3(1, -1, 0);
    gradients[5] = vec3(1, 1, 0);
    gradients[6] = vec3(-1, 1, 0);
    gradients[7] = vec3(0, 1, 1);
    gradients[8] = vec3(-1, 0, -1);
    gradients[9] = vec3(1, 1, 0);
    gradients[10] = vec3(-1, 1, 0);
    gradients[11] = vec3(-1, -1, 0);
    gradients[12] = vec3(1, 0, 1);
    gradients[13] = vec3(-1, 0, 1);
    gradients[14] = vec3(0, -1, 1);
    gradients[15] = vec3(0, -1, -1);

    for (i=0;i<16;i++) table[i]=i;
}


float smoothingFunc(float t)
{
    t = (t > 0.) ? t : -t;

    float t3 = t * t * t;
    float t4 = t3 * t;

    return -6 * t4 * t + 15 * t4 - 10 * t3 + 1.;
}


float randomNumber(float u, float v, float w, int i, int j, int k)
{
    int idx;
    idx = table[abs(k) % 16];
    idx = table[abs(j + idx) % 16];
    idx = table[abs(i + idx) % 16];

    vec3 gijk = gradients[idx];
    vec3 uvw = vec3(u, v, w);

    return smoothingFunc(u) * smoothingFunc(v) * smoothingFunc(w) * dot(gijk, uvw);
}

float perlin(vec3 pos, float scalingFactor)
{
    float x = scalingFactor * pos.x;
    float y = scalingFactor * pos.y;
    float z = scalingFactor * pos.z;

    int xmin = int(floor(x));
    int ymin = int(floor(y));
    int zmin = int(floor(z));

    float n = 0;
    for (int i = xmin; i <= xmin + 1; ++i)
    {
        for (int j = ymin; j <= ymin + 1; ++j)
        {
            for (int k = zmin; k <= zmin + 1; ++k)
            {
                n += randomNumber(x - i, y - j, z - k, i, j, k);
            }
        }
    }

    //return (n + 1.) / 2.;
    return abs(n);
}

void main() {

    vec3 wire_color = vec3(0, 0, 1);
    vec3 dist_vec = dist;

    float shortest_edge_d = min(dist_vec[0], min(dist_vec[1], dist_vec[2]));

    float line_intensity = exp2(-2.0*shortest_edge_d*shortest_edge_d);

    vec3 frag_rgb = line_intensity * wire_color + (1.0 - line_intensity) * Color;

    // Disables wireframe
    frag_rgb = Color;

    float a = 0;

    init();
    for (int i = 0; i <= 3; ++i) {
        a += perlin(TexCoord, pow(2, i)) * sin(TexCoord.x + cos(TexCoord.y));
    }
    a = pow(a, 2); // increase the power to decrease cloud size
    a = (log(a) - log(1e-1)) / (log(1) - log(1e-1)); // smoothing function

    if (a < 0.3){
        a = 1-a;
    }
    frag_color = vec4(frag_rgb*a, 1.0);
}
