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

void main() {


    // "Normal" internal reflection and refraction
    vec3 reflected_eye, refracted_eye;
    get_refract_reflect(Normal, reflected_eye, refracted_eye);

    vec3 color_refracted = texture(envCubeMap, refracted_eye).xyz;
    vec3 color_reflected = texture(envCubeMap, reflected_eye).xyz;

    vec3 mixed_norm_color = mix(color_refracted, color_reflected, FresnelTerm);



    // "Cracked" internal reflection and refraction, with normal chosen from texture
    vec3 crack_position = normalize(Position);
    vec3 crack_norm = texture(IceCrackNormalTex, crack_position.xz).xyz;

    vec3 refl_crack, refract_crack;
    get_refract_reflect(crack_norm, refl_crack, refract_crack);

    vec3 refract_crack_ref_color = texture(envCubeMap, refract_crack).xyz;
    vec3 reflect_crack_ref_color = texture(envCubeMap, refl_crack).xyz;
    vec3 mixed_crack_color = mix(refract_crack_ref_color, reflect_crack_ref_color, FresnelTerm);

    // Choose specific fragment color
    FragColor = vec4(mixed_norm_color, 1.0);
    FragColor = vec4(mixed_crack_color, 1.0);
}
