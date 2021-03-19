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
struct PointLight
{
    vec3 Position;
    float Range;
    float Brightness;
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
    //normalize normal
    vec3 normal = normalize(vNormal);
    //find view vector
    vec3 view = normalize(CameraPosition - vPosition.xyz);

    
    //find the total diffuse and specular of all lights
    vec3 diffuseTotal = vec3(0);
    vec3 specularTotal = vec3(0);
    // --- Directional Lights ---
    for (int i = 0; i < DirectionalLightCount; i++)
    {
        //normalise light direction
        vec3 directionalLightDir = normalize(DirectionalLights[i].Direction);
        //find reflection vector
        vec3 directionalLightReflection = reflect(directionalLightDir, normal);

        //lambert term * light color
        diffuseTotal += max(0, min(1, dot(normal, -directionalLightDir))) * DirectionalLights[i].Color;
        //specular term * light color
        specularTotal += pow(max(0, dot(directionalLightReflection, view)), Ns) * DirectionalLights[i].Color;
    }
    // --- Point Lights ---
    for (int i = 0; i < PointLightCount; i++)
    {
        //find direction from point light to position
        vec3 pointLightDir = vPosition.xyz - PointLights[i].Position;

        //find the magnitude of the direction to get the distance
        float distToLight = sqrt((pointLightDir.x * pointLightDir.x) + (pointLightDir.y * pointLightDir.y) + (pointLightDir.z * pointLightDir.z));
        //if out of range of the point light, skip it
        if (distToLight > PointLights[i].Range)
        {
            continue;
        }

        //normalize direction
        pointLightDir = normalize(pointLightDir);
        //find reflection vector
        vec3 pointLightReflection = reflect(pointLightDir, normal);

        //intensity decreases with distance
        float intensity = 1 - (distToLight / PointLights[i].Range);
        intensity *= PointLights[i].Brightness;

        //lambert term * light color * intensity
        diffuseTotal += max(0, min(1, dot(normal, -pointLightDir))) * PointLights[i].Color * intensity;
        //specular term * light color * intensity
        specularTotal += pow(max(0, dot(pointLightReflection, view)), Ns) * PointLights[i].Color * intensity;
    }

    
    //find the ambient, diffuse, and specular
    vec3 ambient = AmbientColor * Ka;
    vec3 diffuse = diffuseTotal * Kd;
    vec3 specular = specularTotal * Ks;

    //output the final color
    FragColor = vec4(ambient + diffuse + specular + Ke, opacity);
}