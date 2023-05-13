#version 410

in vec3 Position;
in vec3 Normal;
in vec2 TexCoord;
in vec3 Color;

in vec3 ReflectDir;
in vec3 WorldEyeVec;
in float FresnelTerm;

layout (location=0) out vec4 FragColor;

uniform samplerCube envCubeMap;
uniform samplerCube normCubeMap;

const float a = 0.33;
const float eta = 0.7641;

void main() {

    // Potential room for error
    vec3 norm_backface = texture(normCubeMap, WorldEyeVec).xyz;

    vec3 n_r = mix(Normal, norm_backface, a);

    float nr_e_2 = pow(dot(n_r, WorldEyeVec), 2);
    float root_exp = sqrt(1 - (pow(eta, 2) * (1 - nr_e_2)));
    vec3 ref_eye_sub = (eta * dot(n_r, WorldEyeVec) + root_exp) * n_r;

    vec3 refracted_eye = (eta * WorldEyeVec) - ref_eye_sub;
    vec3 color_refracted = texture(envCubeMap, refracted_eye).xyz;

    vec3 reflected_eye = WorldEyeVec - (2*dot(WorldEyeVec, Normal)*Normal);
    vec3 color_reflected = texture(envCubeMap, reflected_eye).xyz;

    vec3 mixed_color = mix(color_refracted, color_reflected, FresnelTerm);

    FragColor = vec4(mixed_color, 1.0);
    //FragColor = texture(normCubeMap, Normal);
    //FragColor = vec4(Normal, 1.0);
}
