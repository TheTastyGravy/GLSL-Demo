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


    //find the total diffuse and specular of all lights
    vec3 diffuseTotal = vec3(0);
    vec3 specularTotal = vec3(0);
    // --- Directional lights ---
    for (int i = 0; i < DirectionalLightCount; i++)
    {
        //normalise the direction vectors
        vec3 directionalLightDir = normalize(DirectionalLights[i].Direction);
        //find the reflection vector
        vec3 directionalLightReflection = reflect(directionalLightDir, N);

        //lambert term * light color
        diffuseTotal += max(0, min(1, dot(N, -directionalLightDir))) * DirectionalLights[i].Color;
        //specular term * light color
        specularTotal += pow(max(0, dot(directionalLightReflection, V)), Ns) * DirectionalLights[i].Color;
    }
    // --- Point lights ---
    for (int i = 0; i < PointLightCount; i++)
    {
        //find direction from point light to position
        vec3 pointLightDir = normalize(vPosition.xyz - PointLights[i].Position);
        //find the reflection vector
        vec3 pointLightReflection = reflect(pointLightDir, N);

        //lambert term * light color
        diffuseTotal += max(0, min(1, dot(N, -pointLightDir))) * PointLights[i].Color;
        //specular term * light color
        specularTotal += pow(max(0, dot(pointLightReflection, V)), Ns) * PointLights[i].Color;
    }


    //find the ambient, diffuse, and specular colors
    vec3 ambient = AmbientColor * Ka * texDiffuse;
    vec3 diffuse = diffuseTotal * Kd * texDiffuse;
    vec3 specular = specularTotal * Ks * texSpecular;

    //output the final color
    FragColor = vec4(ambient + diffuse + specular + Ke, opacity);
}