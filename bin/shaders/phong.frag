// phong shader for simple game lighting
#version 410

in vec4 vPosition;
in vec3 vNormal;

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
//lighting
uniform vec3 AmbientColor;
#define MAX_LIGHTS 16
uniform int DirectionalLightCount;
uniform DirectionalLight DirectionalLights[MAX_LIGHTS];

//used for specular
uniform vec3 CameraPosition;

out vec4 FragColor;


void main()
{
    //normalize vectors
    vec3 N = normalize(vNormal);
    //normalize light directions
    vec3 L[MAX_LIGHTS];
    for (int i = 0; i < DirectionalLightCount; i++)
    {
        L[i] = normalize(DirectionalLights[i].Direction);
    }

    //find view and reflection vectors
    vec3 V = normalize(CameraPosition - vPosition.xyz);
    vec3 R[MAX_LIGHTS];
    for (int i = 0; i < DirectionalLightCount; i++)
    {
        R[i] = reflect(L[i], N);
    }

    vec3 lambertColor = vec3(0);
    vec3 specularColor = vec3(0);
    for (int i = 0; i < DirectionalLightCount; i++)
    {
        //lambert term * light color
        lambertColor += max(0, min(1, dot(N, -L[i]))) * DirectionalLights[i].Color;
        //specular term * light color
        specularColor += pow(max(0, dot(R[i], V)), Ns) * DirectionalLights[i].Color;
    }


    //find the ambient
    vec3 ambient = AmbientColor * Ka;
    //find the diffuse
    vec3 diffuse = lambertColor * Kd;
    //find the specular
    vec3 specular = specularColor * Ks;

    //output the final color
    FragColor = vec4(ambient + diffuse + specular + Ke, opacity);
}