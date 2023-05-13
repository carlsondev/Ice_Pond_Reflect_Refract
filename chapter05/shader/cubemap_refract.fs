#version 430

in vec3 ReflectDir;
in vec3 RefractDir;
in vec2 TexCoord;
in vec3 worldNorm;

layout(binding=0) uniform samplerCube CubeMapTex;
layout(binding=2) uniform sampler2D IceTex;

uniform struct LightInfo {
  vec4 Position; // Light position in eye coords.
  vec3 La;       // Ambient light intensity
  vec3 Ld;       // Diffuse light intensity
  vec3 Ls;       // Specular light intensity
} Light;

struct MaterialInfo {
    float Eta;              // Index of refraction
    float ReflectionFactor; // Percentage of reflected light
    vec3 Ks;            // Specular reflectivity
    float Shininess;    // Specular shininess factor
} Material;

layout( location = 0 ) out vec4 FragColor;

vec3 blinnPhongHighlight(vec3 lightDir, vec3 viewDir, vec3 normal) {
    vec3 halfwayDirection = normalize(lightDir+viewDir);
    // phong: angle between 
    return Light.Ls * Material.Ks *
            pow( max( dot(normal,halfwayDirection), 0.0 ), Material.Shininess );
};

void main() {

    // specular light
    vec3 lightDirection = vec3(0, 1, 0);
    float sDotN = max( dot(lightDirection,worldNorm), 0.0 );
    vec3 specular = vec3(0.0);
    vec3 viewDirection = normalize(-texture(IceTex, TexCoord ).rgb);
    if (sDotN > 0.0) {
        specular = blinnPhongHighlight(lightDirection, viewDirection, worldNorm);
    }

    // Access the cube map texture
    vec3 reflectColor = texture(CubeMapTex, ReflectDir).rgb;
    vec3 refractColor = texture(CubeMapTex, RefractDir).rgb;

    // Add in ice texture
    vec3 baseIceColor = mix(vec3(1.0, 1.0, 1.0), specular, 0.0);
    vec3 iceColor = mix(baseIceColor, texture(IceTex, TexCoord ).rgb, 0.50);

    vec3 color = mix(refractColor, reflectColor, Material.ReflectionFactor);
    color = mix(color, iceColor, 0.9);
    
    // Gamma
    color = pow(color, vec3(1.0/2.2));
    
    FragColor = vec4(color , 1);
}
