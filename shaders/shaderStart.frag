#version 410 core

in vec3 fNormal;
in vec4 fPosEye;
in vec2 fTexCoords;
in vec4 fPosLightSpace;

out vec4 fColor;

uniform sampler2D diffuseTexture;
uniform sampler2D shadowMap;

//Lighting & Spotlight
uniform vec3 lightDir;
uniform vec3 lightColor;
uniform vec3 spotPos;
uniform vec3 spotDir;
uniform vec3 spotColor;
uniform float cutOff;
uniform float outerCutOff;
uniform float constant;
uniform float linear;
uniform float quadratic;

//Uniforms key Flat/Smooth/Fog
uniform bool flatShading;
uniform bool fogEnabled;
uniform vec3 fogColor;
uniform float fogDensity;

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal) {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    if(projCoords.z > 1.0) return 0.0;
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    return currentDepth - bias > closestDepth ? 1.0 : 0.0;
}

void main() {
    vec3 normalEye;
    if (flatShading) {
        normalEye = normalize(cross(dFdx(fPosEye.xyz), dFdy(fPosEye.xyz)));
    } else {
        normalEye = normalize(fNormal);
    }

    vec3 color = texture(diffuseTexture, fTexCoords).rgb;
    vec3 lightDirEye = normalize(lightDir);

    // Directional Lighting
    vec3 ambient = 0.3 * lightColor * color;
    float diff = max(dot(normalEye, lightDirEye), 0.0);
    vec3 diffuse = diff * lightColor * color;
    float shadow = ShadowCalculation(fPosLightSpace, normalEye);
    vec3 directionalResult = ambient + (1.0 - shadow) * diffuse;

    // Spotlight
    vec3 lightDirSpot = normalize(spotPos - fPosEye.xyz);
    float theta = dot(lightDirSpot, normalize(-spotDir));
    float epsilon = cutOff - outerCutOff;
    float intensity = clamp((theta - outerCutOff) / epsilon, 0.0, 1.0);
    float diffSpot = max(dot(normalEye, lightDirSpot), 0.0);
    float dist = length(spotPos - fPosEye.xyz);
    float attenuation = 1.0 / (constant + linear * dist + quadratic * (dist * dist));
    vec3 spotLightResult = diffSpot * spotColor * color * intensity * attenuation;

    vec3 finalColor = directionalResult + spotLightResult;

    if(fogEnabled) {
        float distFog = length(fPosEye.xyz);
        float fogFactor = exp(-pow(distFog * fogDensity, 2));
        fogFactor = clamp(fogFactor, 0.0, 1.0);
        fColor = vec4(mix(fogColor, finalColor, fogFactor), 1.0);
    } else {
        fColor = vec4(finalColor, 1.0);
    }
}