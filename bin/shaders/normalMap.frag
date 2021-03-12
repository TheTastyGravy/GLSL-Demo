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

struct Light
{
    vec3 Direction;
    vec3 Color;
};
//lighting
uniform vec3 AmbientColor;
#define MAX_LIGHTS 16
uniform int LightCount;
uniform Light Lights[MAX_LIGHTS];

//used for specular
uniform vec3 CameraPosition;

out vec4 FragColor;


void main()
{
    //normalize vectors
    vec3 N = normalize(vNormal);
    vec3 T = normalize(vTangent);
    vec3 B = normalize(vBiTangent);
    //normalize light directions
    vec3 L[MAX_LIGHTS];
    for (int i = 0; i < LightCount; i++)
    {
        L[i] = normalize(Lights[i].Direction);
    }

    mat3 TBN = mat3(T, B, N);

    //get pixel from textures
    vec3 texDiffuse = texture(diffuseTexture, vTexCoord).rgb;
    vec3 texSpecular = texture(specularTexture, vTexCoord).rgb;
    vec3 texNormal = texture(normalTexture, vTexCoord).rgb;

    //apply normal map to vertex normal
    N = TBN * (texNormal * 2 - 1);

    //find view and reflection vectors
    vec3 V = normalize(CameraPosition - vPosition.xyz);
    vec3 R[MAX_LIGHTS];
    for (int i = 0; i < LightCount; i++)
    {
        R[i] = reflect(L[i], N);
    }

    vec3 lambertColor = vec3(0);
    vec3 specularColor = vec3(0);
    for (int i = 0; i < LightCount; i++)
    {
        //lambert term * light color
        lambertColor += max(0, min(1, dot(N, -L[i]))) * Lights[i].Color;
        //specular term * light color
        specularColor += pow(max(0, dot(R[i], V)), Ns) * Lights[i].Color;
    }


    //find the ambient
    vec3 ambient = AmbientColor * Ka * texDiffuse;
    //find the diffuse
    vec3 diffuse = lambertColor * Kd * texDiffuse;
    //find the specular
    vec3 specular = specularColor * Ks * texSpecular;

    //output the final color
    FragColor = vec4(ambient + diffuse + specular + Ke, opacity);
}