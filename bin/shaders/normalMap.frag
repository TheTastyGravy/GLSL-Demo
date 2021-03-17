// textured shader for simple game lighting
#version 410

in vec4 vPosition;
in vec3 vNormal;
in vec2 vTexCoord;
in vec3 vTangent;
in vec3 vBiTangent;

uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D normalTexture;

uniform vec3 Ka;    //ambient color
uniform vec3 Kd;    //diffuse color
uniform vec3 Ks;    //specular color
uniform vec3 Ke;    //emissive color
uniform float Ns;   //specular power
uniform float opacity;

struct DirectionalLight
{
    vec3 Direction;
    vec3 Color;
};
struct PointLight
{
    vec3 Position;
    vec3 Color;
};
//lighting
uniform vec3 AmbientColor;
#define MAX_LIGHTS 16
uniform int DirectionalLightCount;
uniform DirectionalLight DirectionalLights[MAX_LIGHTS];
uniform int PointLightCount;
uniform PointLight PointLights[MAX_LIGHTS];

//used for specular
uniform vec3 CameraPosition;

out vec4 FragColor;


void main()
{
    //get pixel from textures
    vec3 texDiffuse = texture(diffuseTexture, vTexCoord).rgb;
    vec3 texSpecular = texture(specularTexture, vTexCoord).rgb;
    vec3 texNormal = texture(normalTexture, vTexCoord).rgb;

    //normalize vectors
    vec3 N = normalize(vNormal);
    vec3 T = normalize(vTangent);
    vec3 B = normalize(vBiTangent);

    //apply normal map to vertex normal
    N = mat3(T, B, N) * (texNormal * 2 - 1);
    //find view vector
    vec3 V = normalize(CameraPosition - vPosition.xyz);


    // --- Directional lights ---
    vec3 directionalLightDirs[MAX_LIGHTS];
    vec3 directionalLightReflections[MAX_LIGHTS];
    for (int i = 0; i < DirectionalLightCount; i++)
    {
        //normalise the direction vectors
        directionalLightDirs[i] = normalize(DirectionalLights[i].Direction);
        //find the reflection vector
        directionalLightReflections[i] = reflect(directionalLightDirs[i], N);
    }
    // --- Point lights ---
    vec3 pointLightDirs[MAX_LIGHTS];
    vec3 pointLightReflections[MAX_LIGHTS];
    for (int i = 0; i < PointLightCount; i++)
    {
        pointLightDirs[i] = normalize(vec3(vPosition.x, vPosition.y, vPosition.z) - PointLights[i].Position);
        pointLightReflections[i] = reflect(pointLightDirs[i], N);
    }


    //use the sum of all lights
    vec3 lambertColor = vec3(0);
    vec3 specularColor = vec3(0);
    for (int i = 0; i < DirectionalLightCount; i++)
    {
        //lambert term * light color
        lambertColor += max(0, min(1, dot(N, -directionalLightDirs[i]))) * DirectionalLights[i].Color;
        //specular term * light color
        specularColor += pow(max(0, dot(directionalLightReflections[i], V)), Ns) * DirectionalLights[i].Color;
    }
    for (int i = 0; i < PointLightCount; i++)
    {
        //lambert term * light color
        lambertColor += max(0, min(1, dot(N, -pointLightDirs[i]))) * PointLights[i].Color;
        //specular term * light color
        specularColor += pow(max(0, dot(pointLightReflections[i], V)), Ns) * PointLights[i].Color;
    }


    //find the ambient, diffuse, and specular colors
    vec3 ambient = AmbientColor * Ka * texDiffuse;
    vec3 diffuse = lambertColor * Kd * texDiffuse;
    vec3 specular = specularColor * Ks * texSpecular;

    //output the final color
    FragColor = vec4(ambient + diffuse + specular + Ke, opacity);
}