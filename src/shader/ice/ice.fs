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

uniform sampler2D IceCrackNormalTex;

uniform bool useCrackModel;

const float a = 0.33;
const float eta = 0.7641;

void get_refract_reflect(vec3 normal, out vec3 reflect, out vec3 refract){
    vec3 norm_backface = texture(normCubeMap, WorldEyeVec).xyz;

    vec3 n_r = mix(normal, norm_backface, a);

    float nr_e_2 = pow(dot(n_r, WorldEyeVec), 2);
    float root_exp = sqrt(1 - (pow(eta, 2) * (1 - nr_e_2)));
    vec3 ref_eye_sub = (eta * dot(n_r, WorldEyeVec) + root_exp) * n_r;

    refract = (eta * WorldEyeVec) - ref_eye_sub;

    reflect = WorldEyeVec - (2*dot(WorldEyeVec, normal)*normal);
}

vec3 get_color_for_normal(vec3 normal){
    vec3 reflected_eye, refracted_eye;
    get_refract_reflect(normal, reflected_eye, refracted_eye);

    vec3 color_refracted = texture(envCubeMap, refracted_eye).xyz;
    vec3 color_reflected = texture(envCubeMap, reflected_eye).xyz;

    vec3 mixed_norm_color = mix(color_refracted, color_reflected, FresnelTerm);
    return mixed_norm_color;
}

void main() {

    if (useCrackModel){
        // "Cracked" internal reflection and refraction, with normal chosen from texture
        vec3 crack_position = normalize(Position);
        vec3 crack_norm = texture(IceCrackNormalTex, crack_position.xz).xyz;

        vec3 mixed_crack_color = get_color_for_normal(crack_norm);

        FragColor = vec4(mixed_crack_color, 1.0);
    }else{
        // "Normal" internal reflection and refraction
        vec3 mixed_norm_color = get_color_for_normal(Normal);

        FragColor = vec4(mixed_norm_color, 1.0);
    }
}
