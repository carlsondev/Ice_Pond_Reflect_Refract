#version 410

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 4) in vec3 VertexColor;

out vec3 Position;
out vec3 Normal;
out vec3 Color;

uniform struct LightInfo {
    vec3 lightColor;
    vec3 lightPos;
    vec3 ambientIntensity;       // Ambient light intensity
    vec3 diffuseIntensity;       // Diffuse light intensity
    vec3 specularIntensity;       // Specular light intensity
} Light;

uniform struct MaterialInfo {
    float shininess;
    vec3 ambientReflectivity;       // Ambient light intensity
    vec3 diffuseReflectivity;       // Diffuse light intensity
    vec3 specularReflectivity;       // Specular light intensity
} Material;

uniform vec3 cameraPos;

uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 MVP;


subroutine float specularValueModel(vec3 normal, vec3 lightDirection, vec3 viewDir);
subroutine uniform specularValueModel getSpecularValue;

subroutine(specularValueModel)
float phongSpecularValue(vec3 normal, vec3 lightDirection, vec3 viewDir){
    vec3 reflectDir = reflect(-lightDirection, normal);

    return pow(max(dot(viewDir, reflectDir), 0.0), Material.shininess);
}

subroutine(specularValueModel)
float blinnPhongSpecularValue(vec3 normal, vec3 lightDirection, vec3 viewDir){

    vec3 halfwayDir = normalize(lightDirection + viewDir);

    return pow(max(dot(normal, halfwayDir), 0.0), Material.shininess);
}


vec3 applyIllumination(){

    vec3 normal = normalize(Normal);
    vec3 lightDirection = normalize(Light.lightPos - Position);
    vec3 viewDir = normalize(cameraPos - Position);

    float directionNormalAngle = max(dot(lightDirection,normal), 0);

    vec3 illumDiffuse = Light.diffuseIntensity * Material.diffuseReflectivity * directionNormalAngle;
    vec3 illumAmbient = Light.ambientIntensity * Material.ambientReflectivity;

    vec3 specular = vec3(0.0);
    if (directionNormalAngle > 0){
        float specularVal = getSpecularValue(normal, lightDirection, viewDir);
        specular = Light.specularIntensity * Material.specularReflectivity * specularVal;
    }

    return illumDiffuse + illumAmbient + specular;
}

void main() {
    Normal = normalize( NormalMatrix * VertexNormal);
    Position = ( ModelViewMatrix * vec4(VertexPosition,1.0) ).xyz;

    gl_Position = MVP * vec4(VertexPosition,1.0);

    // Color calculation
    vec3 pointIllumination = applyIllumination();

    vec3 lightedColor = pointIllumination * Light.lightColor;

    //Point Light
    Color = lightedColor * VertexColor;
}