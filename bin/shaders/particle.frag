// a simple shader
#version 410

in vec4 vParticleColor;

out vec4 FragColor;


void main()
{
    FragColor = vParticleColor;
}