// a simple shader
#version 410

in vec4 ParticleColor;

out vec4 FragColor;


void main()
{
    FragColor = ParticleColor;
}