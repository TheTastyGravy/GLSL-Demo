// textured shader for simple game lighting
#version 410

in vec4 vPosition;
in vec3 vNormal;
in vec2 vTexCoord;

uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;

uniform vec3 Ka;    //ambient color
uniform vec3 Kd;    //diffuse color
uniform vec3 Ks;    //specular color
uniform vec3 Ke;    //emissive color
uniform float Ns;   //specular power
uniform float opacity;

//lighting
uniform vec3 AmbientColor;
uniform vec3 LightColor;
uniform vec3 LightDirection;

//used for specular
uniform vec3 CameraPosition;

out vec4 FragColor;


void main()
{
    //normalize normal and light direction
    vec3 N = normalize(vNormal);
    vec3 L = normalize(LightDirection);

    vec3 texDiffuse = texture(diffuseTexture, vTexCoord).rgb;
    vec3 texSpecular = texture(specularTexture, vTexCoord).rgb;

    //find lambert term, negitive the light direction
    float lambertTerm = max(0, min(1, dot(N, -L)));

    //find view and reflection vectors
    vec3 V = normalize(CameraPosition - vPosition.xyz);
    vec3 R = reflect(L, N);

    //find the specular term
    float specularTerm = pow(max(0, dot(R, V)), Ns);

    //find the ambient
    vec3 ambient = AmbientColor * Ka * texDiffuse;
    //find the diffuse
    vec3 diffuse = LightColor * Kd * lambertTerm * texDiffuse;
    //find the specular
    vec3 specular = LightColor * Ks * specularTerm * texSpecular;

    //output the final color
    FragColor = vec4(ambient + diffuse + specular + Ke, opacity);
}